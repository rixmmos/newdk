-- MySQL dump 10.13  Distrib 5.7.41, for Linux (x86_64)
--
-- Host: localhost    Database: DARKEDEN
-- ------------------------------------------------------
-- Server version	5.7.41

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `Triggers`
--

DROP TABLE IF EXISTS `Triggers`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Triggers` (
  `TriggerID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `TriggerType` enum('NPC','QUEST','MONSTER') NOT NULL DEFAULT 'NPC',
  `NPC` varchar(20) DEFAULT NULL,
  `QuestID` int(10) unsigned DEFAULT '0',
  `Conditions` text NOT NULL,
  `Actions` text NOT NULL,
  PRIMARY KEY (`TriggerID`)
) ENGINE=InnoDB AUTO_INCREMENT=1661 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Triggers`
--
-- WHERE:  TriggerID IN (473,474)

LOCK TABLES `Triggers` WRITE;
/*!40000 ALTER TABLE `Triggers` DISABLE KEYS */;
INSERT INTO `Triggers` VALUES (473,'NPC','Detached NPC 473',0,'ConditionType : AtFirst','ActionType : SetPosition\n		ZoneID : 12\n		X : 100\n		Y : 100\n		Dir : 1\n	'),(474,'NPC','Detached NPC 473',0,'ConditionType : Idle','ActionType : Wander\n		X : 100\n		Y : 100\n		Radius : 100\n		MovePercentage : 99\n	');
/*!40000 ALTER TABLE `Triggers` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2026-04-27  9:46:47
