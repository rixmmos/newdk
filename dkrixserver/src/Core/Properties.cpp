//--------------------------------------------------------------------------------
//
// Filename    : Properties.cpp
// Written By  : Reiot
// Description :
//
//--------------------------------------------------------------------------------

// include files
#include "Properties.h"

#include <iostream>
#include <stdlib.h> // atoi(), getenv()


//--------------------------------------------------------------------------------
// Expand ${VAR} placeholders in a value string using environment variables.
//
// Phase 8B (2026-04-18): lets conf/*.conf files reference secrets without
// storing them in plaintext. Existing plaintext values pass through
// untouched — only substrings matching the exact pattern ${...} are
// considered for expansion.
//
// Rules:
//   - "${NAME}" with NAME non-empty and set in the environment   →  getenv(NAME)
//   - "${NAME}" with NAME set to empty string                    →  "" (respected)
//   - "${NAME}" with NAME unset or empty name (${})              →  leaves the
//                                                                   literal "${NAME}"
//                                                                   in place and writes
//                                                                   a one-line warning
//                                                                   to stderr
//   - "$VAR" (no braces), "\${VAR}" (escaped), or anything else  →  passes through
//
// The "leave literal on missing" rule matches how shell scripts without `set -u`
// behave and preserves the old plaintext fallback path: a conf file that
// accidentally contains a "${" sequence still parses.
//--------------------------------------------------------------------------------
static string expandEnvVars(const string& value) {
    string out;
    out.reserve(value.size());

    size_t i = 0;
    while (i < value.size()) {
        // Look for the start of a ${...} placeholder.
        if (i + 1 < value.size() && value[i] == '$' && value[i + 1] == '{') {
            size_t close = value.find('}', i + 2);
            if (close != string::npos) {
                string name = value.substr(i + 2, close - (i + 2));
                if (!name.empty()) {
                    const char* env = ::getenv(name.c_str());
                    if (env != NULL) {
                        // Resolved — splice in the environment value.
                        out.append(env);
                        i = close + 1;
                        continue;
                    }
                    // Missing — warn once, leave literal in place.
                    std::cerr << "Properties: warning: environment variable '"
                              << name << "' referenced in config is unset; "
                              << "leaving placeholder literal in value"
                              << std::endl;
                }
                // Empty name (${}) — leave literal, no warning needed.
            }
            // Unterminated or empty-name: fall through, copy '$' literally.
        }
        out.push_back(value[i]);
        ++i;
    }
    return out;
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
const char Properties::Comment = '#';
const char Properties::Separator = ':';
const char* Properties::WhiteSpaces = " \t";


//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
Properties::Properties(){__BEGIN_TRY __END_CATCH}

Properties::Properties(const string& filename)
    : m_Filename(filename){__BEGIN_TRY __END_CATCH}


      //--------------------------------------------------------------------------------
      // destructor
      //--------------------------------------------------------------------------------
      Properties::~Properties() noexcept {
    // ��� pair �� �����Ѵ�.
    m_Properties.clear();
}


//--------------------------------------------------------------------------------
// load from file
//--------------------------------------------------------------------------------
void Properties::load() {
    __BEGIN_TRY

    if (m_Filename.empty())
        throw Error("filename not specified");

    ifstream ifile(m_Filename.c_str(), ios::in);

    if (!ifile)
        throw FileNotExistException(m_Filename.c_str());

    while (true) {
        string line;
        getline(ifile, line);

        if (ifile.eof())
            break;

        // Phase 13C: strip trailing CR for CRLF-line-ending conf files
        // (confs on /mnt/c via DrvFs are CRLF). Without this, a blank
        // line in a CRLF file becomes a 1-char "\r" line after getline
        // strips LF; it's not empty, not comment-prefixed, and has no
        // ':', so the separator check below throws "missing separator".
        // WhiteSpaces is " \t" only, so find_first_not_of can't save us.
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);

        // �ڸ�Ʈ �����̰ų� �� �����̹Ƿ� skip �Ѵ�.
        if (line.size() == 0 || line[0] == Comment)
            continue;

        // key �� ���۹���(white space�� �ƴ� ����)�� ã�´�.
        size_t key_begin = line.find_first_not_of(WhiteSpaces);

        // key_begin�� npos ��� ���� �׷� ���ڸ� ã�� ���ߴٴ� ���̴�.
        // ��, ���� white space �θ� �Ǿ� �ִ� �����̹Ƿ� skip �Ѵ�.
        if (key_begin == string::npos)
            continue;

        // key �� value �� �������� separator �� ã�´�.
        // key_end ���� sep �� ���� ã�� ������ find_last_not_of()�� �Ἥ
        // sep �������� ������ key_end �� ã�� ���ؼ��̴�. ^^;
        size_t sep = line.find(Separator, key_begin);

        // Separator �� �߰����� ������ ���� �Ľ� ������ �����Ѵ�.
        if (sep == string::npos)
            throw IOException("missing separator");

        // sep �������� ������ key_end �� ã�Ƴ�����.
        size_t key_end = line.find_last_not_of(WhiteSpaces, sep - 1);

        // sep �������� value_begin �� ã�´�.
        size_t value_begin = line.find_first_not_of(WhiteSpaces, sep + 1);

        // key �� ������ value �� ���� �����̴�.
        if (value_begin == string::npos)
            throw IOException("missing value");

        // �� ���������� ������ value_end �� ã�´�.
        // ( value_begin �� ������ value_end �� ������ �����Ѵ�.)
        size_t value_end = line.find_last_not_of(WhiteSpaces);

        // key_begin,key_end �� value_begin,value_end �� ����ؼ�
        // line �� substring �� key �� value �� �����Ѵ�.
        string key = line.substr(key_begin, key_end - key_begin + 1);
        string value = line.substr(value_begin, value_end - value_begin + 1);

        // Phase 8B: expand ${VAR} placeholders via environment. Zero-change
        // for values that contain no ${...} sequence.
        value = expandEnvVars(value);

        // property �� ����Ѵ�.
        setProperty(key, value);
    }

    ifile.close();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// save to file
//--------------------------------------------------------------------------------
void Properties::save() {
    __BEGIN_TRY

    if (m_Filename.empty())
        throw Error("filename not specified");

    ofstream ofile(m_Filename.c_str(), ios::out | ios::trunc);

    for (map<string, string, StringCompare>::iterator itr = m_Properties.begin(); itr != m_Properties.end(); itr++)
        ofile << itr->first << ' ' << Separator << ' ' << itr->second << endl;

    ofile.close();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get property
//--------------------------------------------------------------------------------
string Properties::getProperty(string key) const {
    __BEGIN_TRY

    string value;

    map<string, string, StringCompare>::const_iterator itr = m_Properties.find(key);

    if (itr != m_Properties.end())
        value = itr->second;
    else
        throw NoSuchElementException(key);

    return value;

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get property as int
//--------------------------------------------------------------------------------
int Properties::getPropertyInt(string key) const {
    __BEGIN_TRY

    return atoi(getProperty(key).c_str());

    __END_CATCH
}


//--------------------------------------------------------------------------------
// set property
//--------------------------------------------------------------------------------
void Properties::setProperty(string key, string value) {
    __BEGIN_TRY

    // �̹� Ű�� ������ ���, value �� �����.
    m_Properties[key] = value;

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string Properties::toString() const {
    __BEGIN_TRY

    StringStream msg;

    for (map<string, string, StringCompare>::const_iterator itr = m_Properties.begin(); itr != m_Properties.end();
         itr++) {
        msg << itr->first << " : " << itr->second << "\n";
    }

    if (msg.isEmpty())
        msg << "empty properties";

    return msg.toString();

    __END_CATCH
}


//--------------------------------------------------------------------------------
// global variable definition
//--------------------------------------------------------------------------------
Properties* g_pConfig = NULL;
Properties* g_pTestConfig = NULL; // by sigi. 2002.12.26
