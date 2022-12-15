-- MySQL dump 10.13  Distrib 8.0.16, for Win64 (x86_64)
--
-- Host: localhost    Database: maze
-- ------------------------------------------------------
-- Server version	8.0.16

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
 SET NAMES utf8mb4 ;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `accel_session`
--

DROP TABLE IF EXISTS `accel_session`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
 SET character_set_client = utf8mb4 ;
CREATE TABLE `accel_session` (
  `accel_session_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `session_id` int(11) NOT NULL,
  `timestamp` int(11) DEFAULT NULL,
  `yaw` float DEFAULT NULL,
  `pitch` float DEFAULT NULL,
  `roll` float DEFAULT NULL,
  `accel_values` json DEFAULT NULL,
  `key_event` json DEFAULT NULL,
  `event` int(11) DEFAULT NULL,
  PRIMARY KEY (`accel_session_id`),
  UNIQUE KEY `idAccel_sessions_UNIQUE` (`accel_session_id`),
  UNIQUE KEY `Unique_Session_Time_Event` (`session_id`,`timestamp`,`event`),
  KEY `FK_sessions_id_idx` (`session_id`),
  CONSTRAINT `FK_session_id` FOREIGN KEY (`session_id`) REFERENCES `mouses_sessions` (`session_id`) ON DELETE RESTRICT ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=2647183 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `batch_calendar`
--

DROP TABLE IF EXISTS `batch_calendar`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
 SET character_set_client = utf8mb4 ;
CREATE TABLE `batch_calendar` (
  `batch_cal_id` int(11) NOT NULL AUTO_INCREMENT,
  `mouse_id` int(11) NOT NULL,
  `id_batches` int(11) NOT NULL,
  `bcal_date` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `bcal_event` varchar(45) DEFAULT NULL,
  PRIMARY KEY (`batch_cal_id`),
  UNIQUE KEY `idbatch_calendar_UNIQUE` (`batch_cal_id`) /*!80000 INVISIBLE */,
  UNIQUE KEY `unique_combo` (`mouse_id`,`bcal_date`,`bcal_event`,`id_batches`) /*!80000 INVISIBLE */,
  KEY `batch_id_fk_idx` (`id_batches`),
  CONSTRAINT `batch_id_fk` FOREIGN KEY (`id_batches`) REFERENCES `mouse_batches` (`id_batches`),
  CONSTRAINT `mouse_id_bcal_fk` FOREIGN KEY (`mouse_id`) REFERENCES `mouses` (`mouses_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `calendarl_event_def`
--

DROP TABLE IF EXISTS `calendarl_event_def`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
 SET character_set_client = utf8mb4 ;
CREATE TABLE `calendarl_event_def` (
  `id_calendarl_event` int(11) NOT NULL AUTO_INCREMENT,
  `calendar_event_name` varchar(45) DEFAULT NULL,
  `calendar_event_comments` varchar(512) DEFAULT NULL,
  `require_value` tinyint(1) DEFAULT NULL,
  PRIMARY KEY (`id_calendarl_event`),
  UNIQUE KEY `name_unique` (`calendar_event_name`)
) ENGINE=InnoDB AUTO_INCREMENT=11 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `calibrations`
--

DROP TABLE IF EXISTS `calibrations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
 SET character_set_client = utf8mb4 ;
CREATE TABLE `calibrations` (
  `calib_id` int(11) NOT NULL AUTO_INCREMENT,
  `calib_date` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `lickport_side` varchar(45) DEFAULT NULL,
  `lickport_type` varchar(45) DEFAULT NULL,
  `content_weight` double DEFAULT NULL,
  `container_weight` double DEFAULT NULL,
  `dropcount` int(11) NOT NULL,
  `drop_vol` float GENERATED ALWAYS AS ((((`content_weight` - `container_weight`) / `dropcount`) * 1000)) VIRTUAL,
  PRIMARY KEY (`calib_id`),
  UNIQUE KEY `calib_id_UNIQUE` (`calib_id`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `device`
--

DROP TABLE IF EXISTS `device`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
 SET character_set_client = utf8mb4 ;
CREATE TABLE `device` (
  `device_id` int(11) NOT NULL,
  `name` varchar(45) DEFAULT NULL,
  `explanation` varchar(45) DEFAULT NULL,
  `default_ip` varchar(18) DEFAULT NULL,
  `default_com` int(1) DEFAULT NULL,
  PRIMARY KEY (`device_id`),
  UNIQUE KEY `device_id_UNIQUE` (`device_id`),
  UNIQUE KEY `name_UNIQUE` (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `event_def`
--

DROP TABLE IF EXISTS `event_def`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
 SET character_set_client = utf8mb4 ;
CREATE TABLE `event_def` (
  `event_origin` varchar(45) DEFAULT NULL,
  `event_value` int(11) NOT NULL,
  PRIMARY KEY (`event_value`),
  UNIQUE KEY `event_value_UNIQUE` (`event_value`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `experimenters`
--

DROP TABLE IF EXISTS `experimenters`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
 SET character_set_client = utf8mb4 ;
CREATE TABLE `experimenters` (
  `experimenter_id` int(10) NOT NULL,
  `experimenter_name` varchar(100) NOT NULL,
  PRIMARY KEY (`experimenter_id`),
  UNIQUE KEY `idexperimenter_id_UNIQUE` (`experimenter_id`),
  UNIQUE KEY `experimenter_name_UNIQUE` (`experimenter_name`) /*!80000 INVISIBLE */
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `json_command`
--

DROP TABLE IF EXISTS `json_command`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
 SET character_set_client = utf8mb4 ;
CREATE TABLE `json_command` (
  `command_id` int(11) NOT NULL AUTO_INCREMENT,
  `command` varchar(45) DEFAULT NULL,
  `explanation` varchar(45) DEFAULT NULL,
  `destination` int(11) DEFAULT NULL,
  `type` varchar(45) DEFAULT NULL,
  PRIMARY KEY (`command_id`),
  UNIQUE KEY `idJsonCommands_UNIQUE` (`command_id`),
  UNIQUE KEY `Command_Destination` (`command`,`destination`),
  KEY `Device_fk_idx` (`destination`),
  CONSTRAINT `Device_fk` FOREIGN KEY (`destination`) REFERENCES `device` (`device_id`) ON DELETE RESTRICT ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=9 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `json_values`
--

DROP TABLE IF EXISTS `json_values`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
 SET character_set_client = utf8mb4 ;
CREATE TABLE `json_values` (
  `value_id` int(11) NOT NULL AUTO_INCREMENT,
  `command_id` int(11) DEFAULT NULL,
  `value` varchar(45) DEFAULT NULL,
  `effect` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`value_id`),
  UNIQUE KEY `idjson_values_UNIQUE` (`value_id`),
  UNIQUE KEY `command_value_unique` (`command_id`,`value`),
  CONSTRAINT `Command_fk` FOREIGN KEY (`command_id`) REFERENCES `json_command` (`command_id`) ON DELETE RESTRICT ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Temporary view structure for view `last_session_view`
--

DROP TABLE IF EXISTS `last_session_view`;
/*!50001 DROP VIEW IF EXISTS `last_session_view`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8mb4;
/*!50001 CREATE VIEW `last_session_view` AS SELECT 
 1 AS `session_id`,
 1 AS `mouses_id`,
 1 AS `mouse_batch`,
 1 AS `session_date`,
 1 AS `session_status`,
 1 AS `session_duration`,
 1 AS `experimenter_id`,
 1 AS `training_set_id`,
 1 AS `winrate`,
 1 AS `rw_licks_per_min`,
 1 AS `LRratio`,
 1 AS `rw_licks`,
 1 AS `fail_licks`,
 1 AS `turns`,
 1 AS `rightLicks`,
 1 AS `leftLicks`,
 1 AS `mean_rw_licktime_millis`,
 1 AS `mean_fail_licktime_millis`,
 1 AS `total_Water`,
 1 AS `Commments`,
 1 AS `dropsize`,
 1 AS `choice`,
 1 AS `fiber`,
 1 AS `FRoller`,
 1 AS `HPulley`,
 1 AS `vsd`,
 1 AS `whisk_vid`,
 1 AS `right_stim`,
 1 AS `left_stim`,
 1 AS `reminders`,
 1 AS `lights_stim`,
 1 AS `lights_rewd`,
 1 AS `choice_randomness`,
 1 AS `stuck_doors`,
 1 AS `nb_rewarded_licks_per_port`,
 1 AS `rs`*/;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `mouse_batches`
--

DROP TABLE IF EXISTS `mouse_batches`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
 SET character_set_client = utf8mb4 ;
CREATE TABLE `mouse_batches` (
  `id_batches` int(11) NOT NULL AUTO_INCREMENT,
  `batch_name` varchar(20) DEFAULT NULL,
  `batch_creation_date` datetime DEFAULT CURRENT_TIMESTAMP,
  `batch_description` varchar(200) DEFAULT NULL,
  PRIMARY KEY (`id_batches`),
  UNIQUE KEY `id_MG_UNIQUE` (`id_batches`)
) ENGINE=InnoDB AUTO_INCREMENT=1025 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `mouse_calendar`
--

DROP TABLE IF EXISTS `mouse_calendar`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
 SET character_set_client = utf8mb4 ;
CREATE TABLE `mouse_calendar` (
  `calendar_id` int(11) NOT NULL AUTO_INCREMENT,
  `mouses_id` int(11) NOT NULL,
  `calendar_date` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `deprivation_state` tinyint(4) DEFAULT NULL,
  `water_maze` int(11) DEFAULT NULL,
  `water_complement` int(11) DEFAULT NULL,
  `weight` float DEFAULT NULL,
  `learning_state` varchar(45) DEFAULT NULL,
  `SpecialEvent_key` int(11) DEFAULT NULL,
  `SpecialEvent_value` int(11) DEFAULT NULL,
  `Commments` varchar(512) DEFAULT NULL,
  PRIMARY KEY (`calendar_id`),
  UNIQUE KEY `calendar_id_UNIQUE` (`calendar_id`),
  UNIQUE KEY `UNIK_mouse_date` (`mouses_id`,`calendar_date`),
  KEY `mouses_id_idx` (`mouses_id`),
  KEY `fk_SpecialEvent_id` (`SpecialEvent_key`),
  CONSTRAINT `FK_mouse_id` FOREIGN KEY (`mouses_id`) REFERENCES `mouses` (`mouses_id`) ON DELETE RESTRICT ON UPDATE CASCADE,
  CONSTRAINT `fk_SpecialEvent_id` FOREIGN KEY (`SpecialEvent_key`) REFERENCES `calendarl_event_def` (`id_calendarl_event`) ON DELETE RESTRICT ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=1690 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `mouses`
--

DROP TABLE IF EXISTS `mouses`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
 SET character_set_client = utf8mb4 ;
CREATE TABLE `mouses` (
  `mouses_id` int(11) NOT NULL AUTO_INCREMENT,
  `mouse_number` int(11) DEFAULT NULL,
  `batch_id` int(11) DEFAULT NULL,
  `birth` date DEFAULT NULL,
  `arrival` date DEFAULT NULL,
  `death` date DEFAULT NULL,
  `mouse_type` varchar(20) DEFAULT 'c57bl6',
  `gender` char(1) DEFAULT NULL,
  `learningState` varchar(20) DEFAULT 'naive',
  `Visual_Identif` varchar(20) DEFAULT NULL,
  PRIMARY KEY (`mouses_id`),
  UNIQUE KEY `idx_mouses_mouse_number` (`mouse_number`),
  KEY `fk_batches` (`batch_id`),
  CONSTRAINT `fk_batches` FOREIGN KEY (`batch_id`) REFERENCES `mouse_batches` (`id_batches`) ON DELETE RESTRICT ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=214 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `mouses_sessions`
--

DROP TABLE IF EXISTS `mouses_sessions`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
 SET character_set_client = utf8mb4 ;
CREATE TABLE `mouses_sessions` (
  `session_id` int(11) NOT NULL AUTO_INCREMENT,
  `mouses_id` int(11) DEFAULT NULL,
  `mouse_batch` int(11) DEFAULT NULL,
  `session_date` datetime DEFAULT NULL,
  `session_status` varchar(40) DEFAULT 'waiting',
  `experimenter_id` int(10) DEFAULT NULL,
  `session_duration` time DEFAULT NULL,
  `training_set_id` int(11) DEFAULT NULL,
  `session_metadata` json DEFAULT NULL,
  `Commments` varchar(512) DEFAULT NULL,
  `winrate` int(11) DEFAULT NULL,
  `rw_licks_per_min` float DEFAULT NULL,
  `LRratio` float DEFAULT NULL,
  `rw_licks` int(11) DEFAULT NULL,
  `fail_licks` int(11) DEFAULT NULL,
  `turns` int(11) DEFAULT NULL,
  `rightLicks` int(8) DEFAULT NULL,
  `leftLicks` int(8) DEFAULT NULL,
  `mean_rw_licktime_millis` int(11) DEFAULT NULL,
  `mean_fail_licktime_millis` int(11) DEFAULT NULL,
  `total_Water` int(11) DEFAULT NULL,
  `dropsize` int(11) DEFAULT NULL,
  `choice` int(11) DEFAULT NULL,
  `fiber` int(3) DEFAULT '0',
  `FRoller` int(11) DEFAULT '0',
  `HPulley` int(11) DEFAULT '0',
  `vsd` int(11) DEFAULT NULL,
  `whisk_vid` int(11) DEFAULT NULL,
  `right_stim` varchar(20) DEFAULT 'vertical90',
  `left_stim` varchar(20) DEFAULT 'smooth',
  `reminders` int(11) DEFAULT NULL,
  `lights_stim` int(11) DEFAULT NULL,
  `lights_rewd` int(11) DEFAULT NULL,
  `choice_randomness` varchar(20) DEFAULT 'FullRand',
  `stuck_doors` int(11) DEFAULT NULL,
  `nb_rewarded_licks_per_port` int(11) DEFAULT NULL,
  PRIMARY KEY (`session_id`),
  UNIQUE KEY `session_date` (`session_date`),
  UNIQUE KEY `mouse_session` (`mouses_id`,`session_id`),
  KEY `FK_training_set_id` (`training_set_id`),
  KEY `fk_id_batches` (`mouse_batch`),
  KEY `fk_experimenters_id` (`experimenter_id`),
  CONSTRAINT `FK_training_set_id` FOREIGN KEY (`training_set_id`) REFERENCES `training_set_def` (`training_set_id`) ON DELETE RESTRICT ON UPDATE CASCADE,
  CONSTRAINT `fk_experimenters_id` FOREIGN KEY (`experimenter_id`) REFERENCES `experimenters` (`experimenter_id`),
  CONSTRAINT `fk_id_batches` FOREIGN KEY (`mouse_batch`) REFERENCES `mouse_batches` (`id_batches`) ON DELETE RESTRICT ON UPDATE CASCADE,
  CONSTRAINT `mouses_sessions_ibfk_1` FOREIGN KEY (`mouses_id`) REFERENCES `mouses` (`mouses_id`)
) ENGINE=InnoDB AUTO_INCREMENT=2916 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `random_type`
--

DROP TABLE IF EXISTS `random_type`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
 SET character_set_client = utf8mb4 ;
CREATE TABLE `random_type` (
  `random_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `random_name` varchar(45) DEFAULT NULL,
  `random_description` varchar(255) DEFAULT NULL,
  `sequences_subset` int(11) DEFAULT NULL,
  PRIMARY KEY (`random_id`),
  UNIQUE KEY `random_id_UNIQUE` (`random_id`),
  UNIQUE KEY `random_name_UNIQUE` (`random_name`),
  UNIQUE KEY `sequences_subset_UNIQUE` (`sequences_subset`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Temporary view structure for view `rapid_session_view`
--

DROP TABLE IF EXISTS `rapid_session_view`;
/*!50001 DROP VIEW IF EXISTS `rapid_session_view`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8mb4;
/*!50001 CREATE VIEW `rapid_session_view` AS SELECT 
 1 AS `session_id`,
 1 AS `mouse`,
 1 AS `batch`,
 1 AS `training_set`,
 1 AS `session_date`,
 1 AS `session_status`,
 1 AS `duration`,
 1 AS `central_events`,
 1 AS `licks`,
 1 AS `licks_per_min`,
 1 AS `lr_stim_ratio`,
 1 AS `water`,
 1 AS `comments`*/;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `reference_weights`
--

DROP TABLE IF EXISTS `reference_weights`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
 SET character_set_client = utf8mb4 ;
CREATE TABLE `reference_weights` (
  `rfw_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `rfw_calendar_id` int(11) NOT NULL,
  `ref_batch_id` int(11) NOT NULL,
  PRIMARY KEY (`rfw_id`),
  UNIQUE KEY `rfw_id_UNIQUE` (`rfw_id`),
  KEY `mref_batch_id_fk` (`ref_batch_id`),
  KEY `calendar_id_fk` (`rfw_calendar_id`),
  CONSTRAINT `calendar_id_fk` FOREIGN KEY (`rfw_calendar_id`) REFERENCES `mouse_calendar` (`calendar_id`) ON DELETE RESTRICT ON UPDATE CASCADE,
  CONSTRAINT `mref_batch_id_fk` FOREIGN KEY (`ref_batch_id`) REFERENCES `mouse_batches` (`id_batches`) ON DELETE RESTRICT ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=184 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sequence_items`
--

DROP TABLE IF EXISTS `sequence_items`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
 SET character_set_client = utf8mb4 ;
CREATE TABLE `sequence_items` (
  `sequence_items_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `side_value` int(11) DEFAULT NULL,
  `doublerand_value` int(11) DEFAULT NULL,
  PRIMARY KEY (`sequence_items_id`),
  UNIQUE KEY `sequence_items_id_UNIQUE` (`sequence_items_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sequences`
--

DROP TABLE IF EXISTS `sequences`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
 SET character_set_client = utf8mb4 ;
CREATE TABLE `sequences` (
  `sequence_id` int(11) NOT NULL,
  `sequence_subset` int(11) DEFAULT NULL,
  `seq_altrate` float DEFAULT NULL,
  `seq_sidebias` float DEFAULT NULL,
  `seq_length` int(11) DEFAULT NULL,
  `seq_doublerand` tinyint(4) DEFAULT NULL,
  PRIMARY KEY (`sequence_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `session_detail`
--

DROP TABLE IF EXISTS `session_detail`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
 SET character_set_client = utf8mb4 ;
CREATE TABLE `session_detail` (
  `id_session_detail` int(11) NOT NULL AUTO_INCREMENT,
  `mouses_id` int(11) DEFAULT NULL,
  `session_id` int(11) DEFAULT NULL,
  `timestamp` int(10) unsigned DEFAULT NULL,
  `event_value` int(11) DEFAULT NULL,
  `trial_id` int(10) unsigned DEFAULT NULL,
  PRIMARY KEY (`id_session_detail`),
  UNIQUE KEY `idsession_detail_UNIQUE` (`id_session_detail`),
  UNIQUE KEY `timestamp_mouse_session_UNIQUE` (`mouses_id`,`session_id`,`timestamp`,`event_value`),
  KEY `mouses_id_idx` (`mouses_id`),
  KEY `session_id_idx` (`session_id`),
  KEY `fk_trialsid` (`trial_id`),
  CONSTRAINT `fk_trialsid` FOREIGN KEY (`trial_id`) REFERENCES `trials_meta` (`trial_id`) ON DELETE RESTRICT ON UPDATE CASCADE,
  CONSTRAINT `mouses_id` FOREIGN KEY (`mouses_id`) REFERENCES `mouses` (`mouses_id`) ON DELETE RESTRICT ON UPDATE CASCADE,
  CONSTRAINT `session_id` FOREIGN KEY (`session_id`) REFERENCES `mouses_sessions` (`session_id`) ON DELETE RESTRICT ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=1332157 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `stim_types`
--

DROP TABLE IF EXISTS `stim_types`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
 SET character_set_client = utf8mb4 ;
CREATE TABLE `stim_types` (
  `id_stim` int(10) unsigned NOT NULL,
  `stim_name` varchar(60) DEFAULT NULL,
  `stim_description` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`id_stim`),
  UNIQUE KEY `id_stim_UNIQUE` (`id_stim`),
  UNIQUE KEY `stim_name_UNIQUE` (`stim_name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `training_set_def`
--

DROP TABLE IF EXISTS `training_set_def`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
 SET character_set_client = utf8mb4 ;
CREATE TABLE `training_set_def` (
  `training_set_id` int(11) NOT NULL AUTO_INCREMENT,
  `training_set_name` varchar(45) DEFAULT NULL,
  `batch` int(11) DEFAULT NULL,
  `creation_date` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `status` varchar(20) DEFAULT 'active',
  `training_set_description` varchar(255) DEFAULT NULL,
  `ts_metadata` json DEFAULT NULL,
  `training_set_comments` varchar(512) DEFAULT NULL,
  `TSI_Reference` int(11) DEFAULT NULL,
  `NextTS_id` int(11) DEFAULT NULL,
  `dropsize` int(11) DEFAULT NULL,
  `choice` int(11) DEFAULT NULL,
  `fiber` int(3) DEFAULT NULL,
  `obstacles` int(3) DEFAULT '0',
  `FRoller` int(11) DEFAULT NULL,
  `HPulley` int(11) DEFAULT NULL,
  `vsd` int(11) DEFAULT NULL,
  `whisk_vid` int(11) DEFAULT NULL,
  `position_track` int(11) DEFAULT NULL,
  `right_stim` varchar(20) DEFAULT 'vertical90',
  `left_stim` varchar(20) DEFAULT 'smooth',
  `bimodal_stim_intensity` int(11) DEFAULT NULL,
  `reminders` int(11) DEFAULT NULL,
  `lights_stim` int(11) DEFAULT NULL,
  `lights_rewd` int(11) DEFAULT NULL,
  `choice_randomness` varchar(20) DEFAULT 'FullRand',
  `stuck_doors` int(11) DEFAULT NULL,
  `error_timeout_sec` int(11) DEFAULT '0',
  `nb_rewarded_licks_per_port` int(11) DEFAULT NULL,
  `imaging_type` varchar(5) DEFAULT NULL,
  PRIMARY KEY (`training_set_id`),
  UNIQUE KEY `training_set_id_UNIQUE` (`training_set_id`)
) ENGINE=InnoDB AUTO_INCREMENT=116 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `training_set_inherit`
--

DROP TABLE IF EXISTS `training_set_inherit`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
 SET character_set_client = utf8mb4 ;
CREATE TABLE `training_set_inherit` (
  `TSI_id` int(11) NOT NULL,
  `TSI_name` varchar(45) DEFAULT NULL,
  `TSI_comments` varchar(512) DEFAULT NULL,
  PRIMARY KEY (`TSI_id`),
  UNIQUE KEY `TSI_id_UNIQUE` (`TSI_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `trials_meta`
--

DROP TABLE IF EXISTS `trials_meta`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
 SET character_set_client = utf8mb4 ;
CREATE TABLE `trials_meta` (
  `trial_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `session_id` int(11) NOT NULL,
  `trial_number` int(10) unsigned NOT NULL,
  `trial_metadata` json DEFAULT NULL,
  PRIMARY KEY (`trial_id`),
  UNIQUE KEY `idtrials_meta_UNIQUE` (`trial_id`) /*!80000 INVISIBLE */,
  KEY `UNIQUE_session_trial` (`session_id`,`trial_number`),
  CONSTRAINT `fk_mousessession` FOREIGN KEY (`session_id`) REFERENCES `mouses_sessions` (`session_id`) ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Final view structure for view `last_session_view`
--

/*!50001 DROP VIEW IF EXISTS `last_session_view`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8mb4 */;
/*!50001 SET character_set_results     = utf8mb4 */;
/*!50001 SET collation_connection      = utf8_general_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`Tim`@`%` SQL SECURITY DEFINER */
/*!50001 VIEW `last_session_view` AS select 1 AS `session_id`,1 AS `mouses_id`,1 AS `mouse_batch`,1 AS `session_date`,1 AS `session_status`,1 AS `session_duration`,1 AS `experimenter_id`,1 AS `training_set_id`,1 AS `winrate`,1 AS `rw_licks_per_min`,1 AS `LRratio`,1 AS `rw_licks`,1 AS `fail_licks`,1 AS `turns`,1 AS `rightLicks`,1 AS `leftLicks`,1 AS `mean_rw_licktime_millis`,1 AS `mean_fail_licktime_millis`,1 AS `total_Water`,1 AS `Commments`,1 AS `dropsize`,1 AS `choice`,1 AS `fiber`,1 AS `FRoller`,1 AS `HPulley`,1 AS `vsd`,1 AS `whisk_vid`,1 AS `right_stim`,1 AS `left_stim`,1 AS `reminders`,1 AS `lights_stim`,1 AS `lights_rewd`,1 AS `choice_randomness`,1 AS `stuck_doors`,1 AS `nb_rewarded_licks_per_port`,1 AS `rs` */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `rapid_session_view`
--

/*!50001 DROP VIEW IF EXISTS `rapid_session_view`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8mb4 */;
/*!50001 SET character_set_results     = utf8mb4 */;
/*!50001 SET collation_connection      = utf8_general_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`Tim`@`%` SQL SECURITY DEFINER */
/*!50001 VIEW `rapid_session_view` AS select 1 AS `session_id`,1 AS `mouse`,1 AS `batch`,1 AS `training_set`,1 AS `session_date`,1 AS `session_status`,1 AS `duration`,1 AS `central_events`,1 AS `licks`,1 AS `licks_per_min`,1 AS `lr_stim_ratio`,1 AS `water`,1 AS `comments` */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2022-12-07 21:12:22
