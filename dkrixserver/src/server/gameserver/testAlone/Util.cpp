

void makeZoneIDList(const string& zoneIDs, list<ZoneID_t>& zoneIDList) {
    __BEGIN_TRY

    size_t a = 0, b = 0;

    //////////////////////////////////////////////
    // 12345,67890,
    // a    ba    b
    //////////////////////////////////////////////
    zoneIDList.clear();
    if (zoneIDs.size() <= 1)
        return;

    do {
        b = zoneIDs.find_first_of(',', a);

        if (b == string::npos)
            break;

        string zoneID = trim(zoneIDs.substr(a, b - a));

        
        zoneIDList.push_back(atoi(zoneID.c_str()));

        a = b + 1;

    } while (b != string::npos && b < zoneIDs.size() - 1);

    __END_CATCH
}
