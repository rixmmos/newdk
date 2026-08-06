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
// Lets conf/*.conf reference secrets without storing them in plaintext.
// Existing plaintext values pass through untouched — only substrings matching
// the exact pattern ${...} are considered for expansion, so this is a strict
// superset of the previous "copy the value verbatim" behaviour.
//
// Rules:
//   "${NAME}", NAME set in the environment  -> spliced in via getenv()
//   "${NAME}", NAME set to the empty string -> respected, expands to ""
//   "${NAME}", NAME unset                   -> literal "${NAME}" kept, plus a
//                                              one-line warning on stderr
//   "${}"                                   -> literal kept, no warning
//   "$NAME" (no braces), "\${NAME}", bare $  -> passed through unchanged
//
// Leaving the literal in place on a missing variable matches shell behaviour
// without `set -u`, and preserves the old fallback: a conf file that happens to
// contain "${" still parses. A server misconfigured this way fails at first SQL
// connect rather than refusing to start — the same loud failure the plaintext
// setup already had.
//
// Nested ${...} inside ${...} is not supported by design; the scanner takes the
// first '}' after '${' as the close. No production conf value nests.
//
// Ported 2026-08-06 from the parked line's Phase 8B (commit 4760660 on tag
// archive/modernization-phases-1-17). Hand-applied rather than cherry-picked:
// this tree's Properties.cpp differs deliberately — WhiteSpaces includes '\r'
// here (CRLF conf files) and the mojibake Korean comments have been stripped.
// Both of those are kept.
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
                    // Missing — warn, leave the literal in place.
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
const char* Properties::WhiteSpaces = " \t\r";


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

        
        if (line.size() == 0 || line[0] == Comment)
            continue;

        
        size_t key_begin = line.find_first_not_of(WhiteSpaces);

        
        
        if (key_begin == string::npos)
            continue;

        
        
        
        size_t sep = line.find(Separator, key_begin);

        
        if (sep == string::npos)
            throw IOException("missing separator");

        
        size_t key_end = line.find_last_not_of(WhiteSpaces, sep - 1);

        
        size_t value_begin = line.find_first_not_of(WhiteSpaces, sep + 1);

        
        if (value_begin == string::npos)
            throw IOException("missing value");

        
        
        size_t value_end = line.find_last_not_of(WhiteSpaces);

        
        
        string key = line.substr(key_begin, key_end - key_begin + 1);
        string value = line.substr(value_begin, value_end - value_begin + 1);

        // Resolve any ${VAR} placeholders against the environment. No-op for
        // values that contain no "${" — see expandEnvVars() above.
        value = expandEnvVars(value);

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
