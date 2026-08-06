-- Test Player account for DarkEden smoke-test login.
-- Run against DARKEDEN db:
--   mysql -uelcastle -pelca110 DARKEDEN < /mnt/c/dev/newdk/create_test_account.sql
--
-- Login creds: testuser / testpass
-- (loginserver.conf has IsNetMarble=0, so the auto-insert branch
--  in CLLoginHandler does not run — the row must exist beforehand.)

INSERT IGNORE INTO Player
    (PlayerID, Password, Name, SSN, SpecialEventCount, Event, creation_date,
     Access, LogOn)
VALUES
    ('testuser', 'testpass', 'testuser', '000000-0000000', 2, '0', CURDATE(),
     'ALLOW', 'LOGOFF');

-- Ensure any leftover ghost LogOn status is reset, in case an earlier crash
-- left the row in LOGON/GAME state.
UPDATE Player
   SET LogOn = 'LOGOFF',
       Access = 'ALLOW',
       CurrentLoginServerID = 0,
       CurrentServerGroupID = 0
 WHERE PlayerID = 'testuser';

-- Sanity check
SELECT PlayerID, Password, Name, Access, LogOn, creation_date
  FROM Player
 WHERE PlayerID = 'testuser';
