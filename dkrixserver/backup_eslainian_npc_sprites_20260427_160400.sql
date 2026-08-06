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
-- Table structure for table `NPC`
--

DROP TABLE IF EXISTS `NPC`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `NPC` (
  `Name` varchar(50) NOT NULL DEFAULT '',
  `NPCID` int(10) unsigned NOT NULL DEFAULT '0',
  `SpriteType` int(10) unsigned NOT NULL DEFAULT '0',
  `Race` int(10) unsigned NOT NULL DEFAULT '0',
  `MainColor` int(10) unsigned NOT NULL DEFAULT '0',
  `SubColor` int(10) unsigned NOT NULL DEFAULT '0',
  `ZoneID` int(10) unsigned NOT NULL DEFAULT '0',
  `ClanType` int(10) unsigned NOT NULL DEFAULT '0',
  `ShowInMinimap` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `Description` text NOT NULL,
  `TaxingCastleZoneID` int(10) unsigned NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `NPC`
--

LOCK TABLES `NPC` WRITE;
/*!40000 ALTER TABLE `NPC` DISABLE KEYS */;
INSERT INTO `NPC` VALUES ('Chris',21,20,0,1,1,2021,0,1,'NPC services',1201),('Smirovich',108,44,0,1,1,2021,0,1,'NPC services',1201),('Johan',101,35,0,1,1,2021,0,1,'NPC services',1201),('Gruber',95,28,0,1,1,2022,0,1,'NPC services',1201),('Yoshihara',100,34,0,1,1,2022,0,1,'NPC services',1201),('Sandra',97,31,0,1,1,2022,0,1,'NPC services',1201),('Bossomuch',110,46,0,1,1,2023,0,1,'NPC services',1201),('Yowa',14,13,0,1,1,2023,0,1,'NPC services',1201),('Han',24,23,0,1,1,2023,0,1,'NPC services',1201),('Ockney',157,55,0,1,1,2023,0,1,'NPC services',1201),('Zack',109,45,0,1,1,12,0,1,'Gun trainer and starter weapons',1201),('Matthew',250,0,0,1,1,12,0,1,'Travel and battle zones',1201),('David',27,26,0,1,1,2001,0,1,'NPC services',1201),('Kevin',20,19,0,1,1,2001,0,1,'NPC services',1201),('Salamander',98,32,0,1,1,2001,0,1,'NPC services',1201),('Sonya Z2002',99,33,0,1,1,2002,0,1,'NPC services',1201),('Crow',102,36,0,1,1,2002,0,1,'NPC services',1201),('Clive',23,22,0,1,1,2002,0,1,'NPC services',1201),('Drake',11,10,0,1,1,2003,0,1,'NPC services',1201),('Spartan',12,11,0,1,1,2003,0,1,'NPC services',1201),('Jan',15,14,0,1,1,2003,0,1,'NPC services',1201),('Smith',103,39,0,1,1,2003,0,1,'NPC services',1201),('Stowe',251,0,0,1,1,2004,0,1,'NPC services',1201),('Christine',22,21,0,1,1,2011,0,1,'NPC services',1201),('Griffin Z2011',96,29,0,1,1,2011,0,1,'NPC services',1201),('Singh',13,12,0,1,1,2011,0,1,'NPC services',1201),('Evgeny',159,54,0,1,1,2012,0,1,'NPC services',1201),('Nicola',160,49,0,1,1,2012,0,1,'NPC services',1201),('Richardson',161,51,0,1,1,2012,0,1,'NPC services',1201),('Cain',18,17,0,1,1,2012,0,1,'NPC services',1201),('Hannibal',25,24,0,1,1,2013,0,1,'NPC services',1201),('Seymour',162,52,0,1,1,2013,0,1,'NPC services',1201),('Rahmanov',163,50,0,1,1,2013,0,1,'NPC services',1201),('Stormov',164,53,0,1,1,2013,0,1,'NPC services',1201),('Bricolacas',92,37,1,1,1,1003,0,1,'NPC services',1202),('Kapatini',93,38,1,1,1,1003,0,1,'NPC services',1202),('Valerie',94,30,1,1,1,1003,0,1,'NPC services',1202),('Sierra',219,65,1,1,1,1003,0,1,'NPC services',1202),('Katon',220,66,1,1,1,1003,0,1,'NPC services',1202),('Kaiser',17,16,0,1,1,12,0,1,'Sword and blade trainer',1201),('Jack',16,109,0,1,1,12,0,1,'Motorcycle dealer',1201),('Tanya',26,25,0,1,1,12,0,1,'Heal and enchant trainer',1201),('Dennis',297,0,0,1,1,2101,0,1,'NPC services',0),('Armamoto',296,0,0,1,1,2102,0,1,'NPC services',0),('Terry',302,0,0,1,1,12,0,1,'Item trader',0),('Nancy',303,1,0,1,1,12,0,1,'Item trader',0),('Mark',298,0,0,1,1,2101,0,0,'NPC services',0),('John',299,0,0,1,1,2101,0,0,'NPC services',0),('Lisa',300,1,0,1,1,2101,0,0,'NPC services',0),('Fabian',304,0,0,1,1,2101,0,0,'NPC services',0),('Jeff',305,0,0,1,1,2101,0,0,'NPC services',0),('Tristan',315,0,0,1,1,2102,0,0,'NPC services',0),('Ellen',316,1,0,1,1,2102,0,0,'NPC services',0),('Patrice Z2102',317,1,0,1,1,2102,0,0,'NPC services',0),('Rebecca',255,61,1,1,1,1003,0,1,'NPC services',1202),('Agnes',362,80,1,1,1,1007,1,1,'NPC services',1202),('Ayman',363,81,1,1,1,1007,1,1,'NPC services',1202),('Kaim',364,82,1,1,1,1007,1,1,'NPC services',1202),('Freyja',365,83,1,1,1,1007,1,1,'NPC services',1202),('Larva',368,37,1,1,1,23,0,1,'NPC services',0),('Rave',370,49,1,1,1,34,0,1,'NPC services',0),('Griffin Z61',427,93,1,1,1,61,0,1,'Blood serum, supplies, storage and repair',1204),('Robin',428,94,0,1,1,64,0,1,'Accessories, storage and repair',1203),('Murphy',429,95,1,1,1,61,0,1,'Item trader and castle travel',1204),('Carmen',430,96,0,1,1,64,0,1,'Healing, supplies and battle zone travel',1203),('Aylin',519,116,0,1,1,1201,0,1,'NPC services',1203),('Allen',521,118,0,1,1,1201,0,1,'NPC services',1203),('Simone',525,122,0,1,1,1201,0,1,'NPC services',1203),('Marie',516,113,1,1,1,1202,0,1,'NPC services',1202),('Beatrice',518,115,1,1,1,1202,0,1,'NPC services',1202),('Heft',254,40,1,1,1,1202,0,1,'NPC services',1202),('Judith',522,119,0,1,1,1203,0,1,'NPC services',1201),('Harold',523,120,0,1,1,1203,0,1,'NPC services',1201),('Brenda',524,121,0,1,1,1203,0,1,'NPC services',1201),('Ami',520,117,1,1,1,1204,0,1,'NPC services',1204),('Moloch',517,114,1,1,1,1204,0,1,'NPC services',1204),('Baron',253,40,1,1,1,1204,0,1,'NPC services',1204),('Flauros',551,125,0,1,1,73,0,1,'NPC services',0),('Fafnir',555,127,0,1,1,71,0,1,'NPC services',0),('Arioc',553,126,0,1,1,73,0,1,'NPC services',0),('Immortal',557,128,0,1,1,71,0,1,'NPC services',0),('Adrian',653,143,2,1,1,1311,66,1,'NPC services',1205),('Zed',654,144,2,1,1,1311,66,1,'NPC services',1206),('Magnescu',655,145,2,1,1,1311,66,1,'NPC services',1206),('Sonya Z1311',656,146,2,1,1,1311,66,1,'NPC services',1205),('Gaston',657,147,2,1,1,1311,66,1,'NPC services',1205),('Boaz',658,148,2,1,1,1311,66,1,'NPC services',1206),('Minestus',675,156,2,1,1,72,66,1,'NPC services',1205),('Eldania',677,158,2,1,1,1206,66,1,'NPC services',1205),('Nicias',679,160,2,1,1,1206,66,1,'NPC services',1205),('Ignia',678,159,2,1,1,1206,66,1,'NPC services',1205),('Peacegate',674,155,2,1,1,72,66,1,'NPC services',1206),('Patrice Z1205',680,161,2,1,1,1205,66,1,'NPC services',1206),('Referee',676,157,2,1,1,1205,66,1,'NPC services',1206),('Roa',681,162,2,1,1,1205,66,1,'NPC services',1206),('CastleAtk306Z1501',306,0,0,1,1,1501,99,1,'NPC services',0),('CastleDef307Z1501',307,0,0,1,1,1501,99,1,'NPC services',0),('CastleAtk306Z1503',306,0,0,1,1,1503,99,1,'NPC services',0),('CastleDef307Z1503',307,0,0,1,1,1503,99,1,'NPC services',0),('CastleAtk749Z1502',749,40,1,1,1,1502,0,1,'NPC services',0),('CastleDef750Z1502',750,40,1,1,1,1502,0,1,'NPC services',0),('CastleAtk749Z1504',749,40,1,1,1,1504,0,1,'NPC services',0),('CastleDef750Z1504',750,40,1,1,1,1504,0,1,'NPC services',0),('CastleAtk751Z1505',751,142,2,1,1,1505,66,1,'NPC services',0),('CastleDef752Z1505',752,142,2,1,1,1505,66,1,'NPC services',0),('CastleAtk751Z1506',751,142,2,1,1,1506,66,1,'NPC services',0),('CastleDef752Z1506',752,142,2,1,1,1506,66,1,'NPC services',0),('Frederick',158,56,0,1,1,64,0,1,'Advancement weapon exchange',0);
/*!40000 ALTER TABLE `NPC` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2026-04-27 13:04:01
