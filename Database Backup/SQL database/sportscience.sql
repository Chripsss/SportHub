-- phpMyAdmin SQL Dump
-- version 5.2.1
-- https://www.phpmyadmin.net/
--
-- Host: 127.0.0.1
-- Generation Time: Jul 09, 2024 at 12:31 PM
-- Server version: 10.4.32-MariaDB
-- PHP Version: 8.2.12

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `sportscience`
--
CREATE DATABASE IF NOT EXISTS `sportscience` DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;
USE `sportscience`;

DELIMITER $$
--
-- Procedures
--
DROP PROCEDURE IF EXISTS `reset_agility`$$
CREATE DEFINER=`root`@`localhost` PROCEDURE `reset_agility` ()   BEGIN
DELETE FROM `agility pole run`;
END$$

DROP PROCEDURE IF EXISTS `reset_reaction_training`$$
CREATE DEFINER=`root`@`localhost` PROCEDURE `reset_reaction_training` ()   BEGIN
DELETE FROM `reaction_training`;
END$$

DROP PROCEDURE IF EXISTS `reset_shooting_accuracy`$$
CREATE DEFINER=`root`@`localhost` PROCEDURE `reset_shooting_accuracy` ()   BEGIN
DELETE FROM `shooting_accuracy`;
END$$

DROP PROCEDURE IF EXISTS `reset_smart_wall`$$
CREATE DEFINER=`root`@`localhost` PROCEDURE `reset_smart_wall` ()   BEGIN
DELETE FROM `smart_wall`;
END$$

DROP PROCEDURE IF EXISTS `reset_sprint_new`$$
CREATE DEFINER=`root`@`localhost` PROCEDURE `reset_sprint_new` ()   BEGIN
DELETE FROM `sprint_new`;
END$$

DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `agility pole run`
--

DROP TABLE IF EXISTS `agility pole run`;
CREATE TABLE `agility pole run` (
  `GameID` int(255) NOT NULL,
  `ID` varchar(55) NOT NULL,
  `PlayerName` varchar(255) NOT NULL,
  `OverallRank` int(11) NOT NULL,
  `Time` float NOT NULL,
  `Speed` float NOT NULL,
  `Penalty` int(11) NOT NULL,
  `FinalTime` float NOT NULL,
  `Age` int(5) NOT NULL,
  `EntryDate` datetime NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `agility pole run`
--

INSERT INTO `agility pole run` (`GameID`, `ID`, `PlayerName`, `OverallRank`, `Time`, `Speed`, `Penalty`, `FinalTime`, `Age`, `EntryDate`) VALUES
(130, '22D9BCFC', 'Tiara', 1, 8.1, 2.47, 5, 18.1, 15, '2024-02-08 10:51:53'),
(131, '22D9BCFC', 'Tiara', 2, 8.54, 2.34, 5, 18.54, 15, '2024-02-08 11:28:14'),
(132, '9EAF0B40', 'Aldrifa', 3, 13.54, 1.48, 5, 23.54, 23, '2024-02-08 12:10:57'),
(133, '9EAF0B40', 'Aldrifa', 4, 16.24, 1.23, 5, 26.24, 23, '2024-02-08 12:12:42');

--
-- Triggers `agility pole run`
--
DROP TRIGGER IF EXISTS `agility_user`;
DELIMITER $$
CREATE TRIGGER `agility_user` BEFORE INSERT ON `agility pole run` FOR EACH ROW BEGIN
    DECLARE v_player_name VARCHAR(255);
    DECLARE v_age INT;

    -- Retrieve Player Name and Age from user identification
    SELECT `PlayerName`, `Age` INTO v_player_name, v_age
    FROM `user`
    WHERE ID = NEW.ID;

    -- Set the Player Name and Age for the new row
    SET NEW.`PlayerName` = v_player_name;
    SET NEW.Age = v_age;
END
$$
DELIMITER ;
DROP TRIGGER IF EXISTS `calculate_final_time`;
DELIMITER $$
CREATE TRIGGER `calculate_final_time` BEFORE INSERT ON `agility pole run` FOR EACH ROW BEGIN
    SET NEW.FinalTime = NEW.Time + (NEW.Penalty * 2);
END
$$
DELIMITER ;
DROP TRIGGER IF EXISTS `newest_entry_agility`;
DELIMITER $$
CREATE TRIGGER `newest_entry_agility` AFTER INSERT ON `agility pole run` FOR EACH ROW BEGIN
    -- Insert the newest entry into the display table
    INSERT INTO `display`.`agility` (`PlayerName`, Time, `Speed`, `Penalty`,`FinalTime`, ID, `EntryDate`, Age, `GameID`)
    SELECT NEW.`PlayerName`, NEW.Time,  NEW.`Speed`, NEW.`Penalty`, NEW.`FinalTime`,NEW.ID, NEW.`EntryDate`, NEW.Age, NEW.`GameID`
    FROM `sportscience`.`agility pole run` AS u
    WHERE u.`PlayerName` = NEW.`PlayerName`
    ORDER BY u.`EntryDate` DESC
    LIMIT 1;

SET @overall_rank = 0;
UPDATE `display`.`agility` AS s
JOIN (
    SELECT `FinalTime`, `EntryDate`, (@overall_rank := @overall_rank + 1) AS `OverallRank`
    FROM `sportscience`.`agility pole run`
    ORDER BY `FinalTime` ASC, `EntryDate` DESC
) AS t ON s.`FinalTime` = t.`FinalTime` AND s.`EntryDate` = t.`EntryDate`
SET s.`OverallRank` = t.`OverallRank`;

END
$$
DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `reaction_training`
--

DROP TABLE IF EXISTS `reaction_training`;
CREATE TABLE `reaction_training` (
  `GameID` int(255) NOT NULL,
  `ID` varchar(55) NOT NULL,
  `PlayerName` varchar(255) NOT NULL,
  `OverallRank` int(11) NOT NULL,
  `Point` int(11) NOT NULL,
  `TotalPoint` int(11) NOT NULL,
  `Wrong` int(11) NOT NULL,
  `Score` float NOT NULL,
  `Accuracy` float NOT NULL,
  `Average` float NOT NULL,
  `Fastest` float NOT NULL,
  `Slowest` float NOT NULL,
  `Age` int(5) NOT NULL,
  `EntryDate` datetime NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `reaction_training`
--

INSERT INTO `reaction_training` (`GameID`, `ID`, `PlayerName`, `OverallRank`, `Point`, `TotalPoint`, `Wrong`, `Score`, `Accuracy`, `Average`, `Fastest`, `Slowest`, `Age`, `EntryDate`) VALUES
(266, '9EAF0B40', 'Aldrifa', 20, 29, 100, 0, 0, 100, 0.645, 0.323, 1.037, 23, '2024-02-08 10:39:17'),
(267, '3A9A6D16', 'Muzakki', 44, 14, 100, 0, 0, 100, 1.316, 0.742, 2.962, 23, '2024-02-08 10:47:45'),
(268, 'C32D2C40', 'Sinta', 39, 16, 100, 0, 0, 100, 1.133, 0.514, 2.77, 2, '2024-02-08 10:55:33'),
(269, 'C32D2C40', 'Sinta', 32, 23, 100, 0, 0, 100, 0.801, 0.434, 1.542, 2, '2024-02-08 10:56:33'),
(270, '3A9A6D16', 'Muzakki', 38, 16, 100, 0, 0, 100, 1.14, 0.483, 3.038, 23, '2024-02-08 10:57:10'),
(271, '3A9A6D16', 'Muzakki', 42, 14, 100, 0, 0, 100, 1.308, 0.674, 2.55, 23, '2024-02-08 10:58:52'),
(272, '9EAF0B40', 'Aldrifa', 46, 11, 100, 0, 0, 100, 1.778, 0.526, 3.732, 23, '2024-02-08 12:11:41'),
(273, '3A9A6D16', 'Muzakki', 49, 7, 100, 0, 0, 100, 2.706, 0.948, 4.548, 23, '2024-02-08 12:13:37'),
(274, '2C649534', 'Janice', 5, 38, 39, 0, 0, 97.44, 0.468, 0.238, 0.907, 20, '2024-02-29 16:39:56'),
(275, '9EAF0B40', 'Aldrifa', 2, 40, 40, 0, 0, 100, 0.442, 0.371, 0.695, 23, '2024-02-29 16:41:53'),
(276, '5AE10FB1', ' IE Wira ', 3, 39, 39, 0, 0, 100, 0.462, 0.359, 0.816, 2, '2024-02-29 16:42:25'),
(277, '9EAF0B40', 'Aldrifa', 25, 26, 26, 0, 0, 100, 0.705, 0.288, 6.944, 23, '2024-02-29 16:43:04'),
(278, '2C649534', 'Janice', 16, 32, 32, 0, 0, 100, 0.547, 0.425, 0.775, 20, '2024-02-29 16:43:39'),
(279, 'D09A9A32', 'Aldrifa', 22, 28, 28, 0, 0, 100, 0.67, 0.42, 1.21, 22, '2024-02-29 17:13:16'),
(280, '729DBEFC', 'Fitri', 21, 28, 32, 0, 0, 87.5, 0.633, 0.009, 1.072, 2, '2024-02-29 17:14:39'),
(281, '729DBEFC', 'Fitri', 18, 30, 30, 0, 0, 100, 0.6, 0.402, 1.298, 2, '2024-03-01 17:01:00'),
(282, 'FAE10CB0', 'ME Rafli', 37, 19, 20, 0, 0, 95, 0.883, 0.101, 1.775, 10, '2024-03-01 17:05:06'),
(283, '1CF2C738', 'Player L', 11, 34, 36, 0, 0, 94.44, 0.508, 0.012, 0.857, 1, '2024-03-02 10:30:41'),
(284, '91EEFA1D', 'Player U ', 7, 36, 36, 0, 0, 100, 0.482, 0.281, 0.802, 22, '2024-03-02 10:32:34'),
(285, '729DBEFC', 'Fitri', 15, 32, 33, 0, 0, 96.97, 0.536, 0.19, 1.078, 2, '2024-03-02 10:33:12'),
(286, '97918B7A', 'Player V', 17, 31, 33, 0, 0, 93.94, 0.581, 0.055, 2.287, 9, '2024-03-02 10:33:54'),
(287, 'E76D9A7B', 'Player AA', 14, 33, 33, 0, 0, 100, 0.536, 0.323, 1.191, 1, '2024-03-02 10:34:25'),
(288, '81EE551D', 'Player M', 6, 36, 46, 0, 0, 78.26, 0.503, 0.01, 0.962, 2, '2024-03-02 10:34:55'),
(289, 'D09A9A32', 'Aldrifa', 8, 35, 46, 0, 0, 76.09, 0.485, 0.009, 0.945, 22, '2024-03-02 10:35:29'),
(290, '3A9A6D16', 'Muzakki', 41, 14, 28, 0, 0, 50, 0.525, 0.009, 0.938, 23, '2024-03-02 10:36:06'),
(291, '9EAF0B40', 'Aldrifa', 24, 27, 31, 0, 0, 87.1, 0.608, 0.024, 1.133, 23, '2024-03-02 10:39:21'),
(292, 'C32D2C40', 'Sinta', 52, 3, 26, 0, 0, 11.54, 0.893, 0.009, 0.765, 2, '2024-03-02 10:41:03'),
(293, 'D09A9A32', 'player J\r\n', 48, 9, 28, 0, 0, 32.14, 0.845, 0.01, 1.326, 22, '2024-03-02 10:51:11'),
(294, '729DBEFC', 'Player G', 29, 23, 24, 0, 0, 95.83, 0.792, 0.01, 1.942, 2, '2024-03-02 10:52:31'),
(295, 'FED96E85', 'Tiara', 31, 23, 24, 0, 0, 95.83, 0.791, 0.12, 1.051, 3, '2024-03-02 10:53:19'),
(296, 'DA73FDAF', 'RO William', 30, 23, 24, 0, 0, 95.83, 0.774, 0.082, 1.648, 6, '2024-03-02 10:54:14'),
(297, '16D57D8D', 'FOB Tina', 35, 20, 24, 0, 0, 83.33, 0.916, 0.009, 1.265, 5, '2024-03-02 10:55:12'),
(298, '06036F90', 'Riki', 26, 26, 26, 0, 0, 100, 0.697, 0.433, 0.97, 3, '2024-03-02 10:56:06'),
(299, 'FAE10CB0', 'ME Rafli', 33, 22, 25, 0, 0, 88, 0.849, 0.011, 1.768, 10, '2024-03-02 10:57:52'),
(300, 'BA1C26B0', 'Pak Gun', 28, 24, 28, 0, 0, 85.71, 0.751, 0.011, 1.968, 22, '2024-03-02 10:59:53'),
(301, 'A15D2F1D', 'Rar', 34, 21, 46, 0, 0, 45.65, 0.851, 0.009, 1.472, 2, '2024-03-02 11:01:20'),
(302, '0C131239', 'Budi', 40, 15, 27, 0, 0, 55.56, 1.365, 0.009, 2.876, 8, '2024-03-02 11:02:20'),
(303, 'C32D2C40', 'Sinta', 48, 9, 25, 0, 0, 36, 1.368, 0.01, 4.955, 2, '2024-03-02 11:15:30'),
(304, 'C32D2C40', 'Sinta', 19, 29, 33, 0, 0, 87.88, 0.574, 0.012, 0.89, 2, '2024-03-02 11:16:00'),
(305, '9EAF0B40', 'Aldrifa', 1, 41, 54, 0, 0, 75.93, 0.447, 0.01, 0.606, 23, '2024-03-02 11:58:53'),
(306, '2637247E', 'Player S', 36, 19, 31, 0, 0, 61.29, 0.6, 0.009, 1.478, 9, '2024-03-02 11:59:47'),
(307, 'DA73FDAF', 'Player P', 12, 34, 42, 0, 0, 80.95, 0.462, 0.031, 0.937, 6, '2024-03-02 12:02:11'),
(308, '9EAF0B40', 'Aldrifa', 10, 34, 40, 0, 0, 85, 0.549, 0.01, 0.862, 23, '2024-03-02 12:42:43'),
(309, '0C131239', 'Player R', 45, 13, 20, 0, 0, 65, 0.821, 0.011, 1.428, 8, '2024-03-02 12:43:51'),
(310, '10A83A0E', 'Nathan Wijaya', 50, 6, 2, 0, 0, 2.97, 606820, 0.009, 3640920, 0, '2024-07-09 11:01:19'),
(311, 'A15D2F1D', 'Player Z', 13, 33, 68, 0, 0, 68.75, 0.541, 0.01, 1.822, 2, '2024-07-09 11:02:47'),
(312, 'C32D2C40', 'Sinta Widianingrum', 27, 25, 51, 0, 0, 51.02, 0.702, 0.009, 1.363, 2, '2024-07-09 11:05:44'),
(313, 'D6776990', 'Player H', 4, 38, 67, 0, 0, 67.86, 0.452, 0.009, 0.667, 2, '2024-07-09 11:06:57'),
(314, '6A59EDAF', 'Player G', 43, 14, 100, 0, 0, 100, 1.176, 0.71, 2.028, 2, '2024-07-09 11:09:03'),
(315, '91EEFA1D', 'Player U ', 23, 27, 81, 0, 0, 81.82, 0.605, 0.01, 1.402, 22, '2024-07-09 11:09:58'),
(316, '91EEFA1D', 'Player U ', 51, 4, 100, 0, 0, 100, 5.691, 2.418, 10.126, 22, '2024-07-09 11:10:46'),
(317, 'FED96E85', 'Aldrifa Farizki', 9, 35, 72, 0, 0, 72.92, 0.487, 0.01, 1.106, 3, '2024-07-09 11:11:19');

--
-- Triggers `reaction_training`
--
DROP TRIGGER IF EXISTS `newest_entry_reaction_training`;
DELIMITER $$
CREATE TRIGGER `newest_entry_reaction_training` AFTER INSERT ON `reaction_training` FOR EACH ROW BEGIN
    -- Insert the newest entry into the display table
 INSERT INTO `display`.`reaction` (`GameID`, `ID`, `PlayerName`,`OverallRank`, `Point`,`TotalPoint`, `Wrong`,`Score`,`Accuracy`,`Average`,`Fastest`,`Slowest`, `Age`,`EntryDate`)  
SELECT NEW.`GameID`,  NEW.`ID`, NEW.`PlayerName`, NEW.`OverallRank`, NEW.`Point`, NEW.`TotalPoint`, NEW.`Wrong`, NEW.`Score`, NEW.`Accuracy`, NEW.`Average`,NEW.`Fastest`,NEW.`Slowest`, NEW.`Age`, NEW.`EntryDate`
    FROM `sportscience`.`reaction_training` AS u
    WHERE u.`PlayerName` = NEW.`PlayerName`
    ORDER BY u.`EntryDate` DESC
    LIMIT 1;

SET @overall_rank = 0;
UPDATE `display`.`reaction` AS s
JOIN (
    SELECT `Point`, `Fastest`, (@overall_rank := @overall_rank + 1) AS `OverallRank`
    FROM `sportscience`.`reaction_training`
    ORDER BY `Point` DESC, `Fastest` ASC
) AS t ON s.`Point` = t.`Point` AND s.`Fastest` = t.`Fastest`
SET s.`OverallRank` = t.`OverallRank`;

END
$$
DELIMITER ;
DROP TRIGGER IF EXISTS `reaction_training_user`;
DELIMITER $$
CREATE TRIGGER `reaction_training_user` BEFORE INSERT ON `reaction_training` FOR EACH ROW BEGIN
    DECLARE v_player_name VARCHAR(255);
    DECLARE v_age INT;

    -- Retrieve Player Name and Age from user identification
    SELECT `PlayerName`, `Age` INTO v_player_name, v_age
    FROM `user`
    WHERE ID = NEW.ID;

    -- Set the Player Name and Age for the new row
    SET NEW.`PlayerName` = v_player_name;
    SET NEW.Age = v_age;
END
$$
DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `shooting_accuracy`
--

DROP TABLE IF EXISTS `shooting_accuracy`;
CREATE TABLE `shooting_accuracy` (
  `GameID` int(255) NOT NULL,
  `ID` varchar(55) NOT NULL,
  `PlayerName` varchar(255) NOT NULL,
  `OverallRank` int(11) NOT NULL,
  `Shoot` int(11) NOT NULL,
  `Goal` int(11) NOT NULL,
  `Miss` int(11) NOT NULL,
  `Accuracy` float NOT NULL,
  `Age` int(5) NOT NULL,
  `EntryDate` datetime NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `shooting_accuracy`
--

INSERT INTO `shooting_accuracy` (`GameID`, `ID`, `PlayerName`, `OverallRank`, `Shoot`, `Goal`, `Miss`, `Accuracy`, `Age`, `EntryDate`) VALUES
(37, 'C32D2C40', 'Sinta', 3, 2, 1, 1, 50, 2, '2024-02-08 10:35:33'),
(38, 'C32D2C40', 'Sinta', 2, 9, 7, 2, 77.78, 2, '2024-02-08 10:36:17'),
(39, '3A9A6D16', 'Muzakki', 4, 1, 0, 1, 0, 23, '2024-02-08 11:02:55'),
(40, '10A83A0E', 'Steven', 1, 11, 8, 3, 72.73, 0, '2024-02-08 11:03:38');

--
-- Triggers `shooting_accuracy`
--
DROP TRIGGER IF EXISTS `newest_entry_shooting_accuracy`;
DELIMITER $$
CREATE TRIGGER `newest_entry_shooting_accuracy` AFTER INSERT ON `shooting_accuracy` FOR EACH ROW BEGIN
    -- Insert the newest entry into the display table
    -- Insert the latest row from shooting into shooting IN DISPLAY DATABASE
    INSERT INTO `display`.`shooting` (`GameID`, `ID`, `PlayerName`, `OverallRank`, `Shoot`, `Goal`,  `Miss`, `Accuracy`, `Age`, `EntryDate`)
    SELECT NEW.`GameID`, NEW.`ID`, NEW.`PlayerName`, NEW.`OverallRank`, NEW.`Shoot`, NEW.`Goal`, NEW.`Miss`, NEW.`Accuracy`, NEW.`Age`, NEW.`EntryDate`
    FROM `sportscience`.`shooting_accuracy` AS u
    WHERE u.`PlayerName` = NEW.`PlayerName`
    ORDER BY u.`EntryDate` DESC
    LIMIT 1;

SET @overall_rank = 0;
UPDATE `display`.`shooting` AS s
JOIN (
    SELECT `Goal`, `Accuracy`, (@overall_rank := @overall_rank + 1) AS `OverallRank`
    FROM `sportscience`.`shooting_accuracy`
    ORDER BY `Goal` DESC, `Accuracy` DESC
) AS t ON s.`Goal` = t.`Goal` AND s.`Accuracy` = t.`Accuracy`
SET s.`OverallRank` = t.`OverallRank`;

END
$$
DELIMITER ;
DROP TRIGGER IF EXISTS `shooting_accuracy_user`;
DELIMITER $$
CREATE TRIGGER `shooting_accuracy_user` BEFORE INSERT ON `shooting_accuracy` FOR EACH ROW BEGIN
    DECLARE v_player_name VARCHAR(255);
    DECLARE v_age INT;

    -- Retrieve Player Name and Age from user identification
    SELECT `PlayerName`, `Age` INTO v_player_name, v_age
    FROM `user`
    WHERE ID = NEW.ID;

    -- Set the Player Name and Age for the new row
    SET NEW.`PlayerName` = v_player_name;
    SET NEW.Age = v_age;
END
$$
DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `smart_wall`
--

DROP TABLE IF EXISTS `smart_wall`;
CREATE TABLE `smart_wall` (
  `GameID` int(255) NOT NULL,
  `ID` varchar(55) NOT NULL,
  `PlayerName` varchar(255) NOT NULL,
  `OverallRank` int(11) NOT NULL,
  `Point` int(11) NOT NULL,
  `Wrong` int(11) NOT NULL,
  `Score` float NOT NULL,
  `Accuracy` float NOT NULL,
  `Average` float NOT NULL,
  `Fastest` float NOT NULL,
  `Slowest` float NOT NULL,
  `Age` int(5) NOT NULL,
  `EntryDate` datetime NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `smart_wall`
--

INSERT INTO `smart_wall` (`GameID`, `ID`, `PlayerName`, `OverallRank`, `Point`, `Wrong`, `Score`, `Accuracy`, `Average`, `Fastest`, `Slowest`, `Age`, `EntryDate`) VALUES
(160, '9EAF0B40', 'Aldrifa', 68, 9, 0, 0, 50, 2.917, 0.309, 11.051, 23, '2024-02-08 10:51:00'),
(161, '3A9A6D16', 'Muzakki', 28, 18, 0, 0, 90, 1.547, 1.026, 2.856, 23, '2024-02-08 11:00:56'),
(162, '2C649534', 'Janice', 73, 8, 0, 0, 13.56, 2.073, 0.79, 3.489, 20, '2024-02-29 16:40:52'),
(163, '9EAF0B40', 'Aldrifa', 82, 2, 0, 0, 0.87, 1.394, 0.203, 2.585, 23, '2024-02-29 16:45:41'),
(164, '9EAF0B40', 'Aldrifa', 71, 8, 0, 0, 7.41, 2.195, 0.2, 8.393, 23, '2024-02-29 16:46:46'),
(165, '5AE10FB1', ' IE Wira ', 81, 4, 0, 0, 80, 7.19, 1.861, 16.194, 2, '2024-02-29 16:52:54'),
(166, '5AE10FB1', ' IE Wira ', 84, 0, 0, 0, 0, 0, 1.861, 16.194, 2, '2024-02-29 16:53:36'),
(167, '5AE10FB1', ' IE Wira ', 80, 5, 0, 0, 3.97, 3.107, 1.841, 4.581, 2, '2024-02-29 16:54:16'),
(168, '9EAF0B40', 'Aldrifa', 79, 5, 0, 0, 1.9, 0.2, 0.199, 0.201, 23, '2024-02-29 16:57:42'),
(169, '9EAF0B40', 'Aldrifa', 83, 1, 0, 0, 0.38, 0.202, 0.202, 0.202, 23, '2024-02-29 16:58:23'),
(170, 'A15D2F1D', 'Rar', 70, 9, 0, 0, 75, 2.885, 1.148, 4.937, 2, '2024-03-01 18:01:39'),
(171, 'C32D2C40', 'Sinta', 76, 7, 0, 0, 77.78, 4.168, 2.387, 7.697, 2, '2024-03-02 10:48:59'),
(172, 'A191321D', 'ME Kush', 54, 14, 0, 0, 87.5, 1.959, 1.108, 7.282, 4, '2024-03-02 10:54:34'),
(173, '2637247E', 'IE FILSCHA', 66, 10, 0, 0, 83.33, 3.059, 1.265, 5.232, 9, '2024-03-02 10:59:55'),
(174, '2637247E', 'Player S', 75, 7, 0, 0, 100, 5.016, 1.938, 9.274, 9, '2024-03-02 11:56:17'),
(175, 'E76D9A7B', 'Player AA', 65, 11, 0, 0, 100, 2.892, 1.584, 5.385, 1, '2024-03-02 12:06:11'),
(176, '97918B7A', 'Player V', 74, 8, 0, 0, 47.06, 2.224, 1.102, 3.682, 9, '2024-03-02 12:20:16'),
(177, 'DA73FDAF', 'Player P', 77, 6, 0, 0, 85.71, 5.969, 1.269, 12.048, 6, '2024-03-02 12:38:50'),
(178, '9EAF0B40', 'Aldrifa', 35, 17, 0, 0, 80.95, 1.541, 0.953, 2.179, 23, '2024-03-22 13:18:01'),
(179, '9EAF0B40', 'Aldrifa', 27, 18, 0, 0, 100, 1.696, 0.955, 2.483, 23, '2024-03-22 13:19:19'),
(180, '9EAF0B40', 'Aldrifa', 33, 17, 0, 0, 85, 1.622, 0.456, 3.084, 23, '2024-03-22 13:20:42'),
(181, '2C649534', 'Janice', 32, 18, 0, 0, 100, 1.683, 1.199, 2.214, 20, '2024-03-22 13:38:14'),
(182, 'C32D2C40', 'Sinta', 64, 11, 0, 0, 78.57, 2.369, 1.325, 4.329, 2, '2024-05-08 13:42:38'),
(183, '2AE43D16', 'FOB Tri', 49, 14, 0, 0, 60.87, 1.748, 0.386, 3.754, 20, '2024-05-08 14:20:12'),
(184, '1CF2C738', 'Player L', 55, 14, 0, 0, 93.33, 2.059, 1.22, 3.62, 1, '2024-05-11 10:34:42'),
(185, '1CF2C738', 'Player L', 37, 16, 0, 0, 88.89, 1.633, 0.946, 2.887, 1, '2024-05-11 11:09:30'),
(186, '0C131239', 'Player R', 20, 19, 0, 0, 100, 1.661, 0.879, 2.998, 8, '2024-05-11 11:34:06'),
(187, '0C131239', 'Player R', 7, 23, 0, 0, 92, 1.207, 0.846, 3.356, 8, '2024-05-11 12:05:35'),
(188, '16D57D8D', 'Player O', 8, 22, 0, 0, 91.67, 1.297, 0.368, 1.841, 5, '2024-05-11 12:09:34'),
(189, '1CF2C738', 'Player L', 22, 18, 0, 0, 94.74, 1.66, 0.695, 2.376, 1, '2024-05-11 12:10:36'),
(190, '76F27E8D', 'Player Y', 19, 19, 0, 0, 82.61, 1.399, 0.85, 2.402, 22, '2024-05-11 12:11:43'),
(191, '2637247E', 'Player S', 45, 15, 0, 0, 88.24, 1.943, 0.959, 3.3, 9, '2024-05-11 12:26:55'),
(192, '06036F90', 'Player K', 39, 16, 0, 0, 84.21, 1.591, 1.05, 3.098, 3, '2024-05-11 12:40:28'),
(193, '5AE10FB1', ' IE Wira ', 42, 15, 0, 0, 78.95, 1.531, 0.87, 2.571, 2, '2024-05-11 12:41:54'),
(194, 'FAE10CB0', 'ME Rafli', 52, 14, 0, 0, 82.35, 1.561, 0.915, 2.747, 10, '2024-05-11 12:47:55'),
(195, '81EE551D', 'Player M', 2, 24, 0, 0, 92.31, 1.192, 0.761, 2.424, 2, '2024-05-11 12:49:04'),
(196, 'A191321D', 'Player N', 13, 20, 0, 0, 83.33, 1.279, 0.486, 1.889, 4, '2024-05-11 12:53:23'),
(197, '0C131239', 'Player R', 3, 24, 0, 0, 88.89, 1.146, 0.773, 1.621, 8, '2024-05-11 12:58:21'),
(198, '729DBEFC', 'Player G', 67, 10, 0, 0, 83.33, 2.501, 1.544, 4.236, 2, '2024-05-11 13:04:44'),
(199, '729DBEFC', 'Player G', 62, 11, 0, 0, 73.33, 2.2, 0.828, 4.231, 2, '2024-05-11 13:11:42'),
(200, '729DBEFC', 'Player G', 21, 18, 0, 0, 100, 1.695, 0.497, 4.561, 2, '2024-05-11 13:12:35'),
(201, '06036F90', 'Player K', 46, 15, 0, 0, 88.24, 1.935, 0.964, 5.031, 3, '2024-05-11 13:33:18'),
(202, '06036F90', 'Player K', 40, 15, 0, 0, 93.75, 1.886, 0.298, 10.387, 3, '2024-05-11 13:34:57'),
(203, '81EE551D', 'Player M', 25, 18, 0, 0, 90, 1.796, 0.818, 6.201, 2, '2024-05-11 13:37:39'),
(204, '2637247E', 'Player S', 31, 18, 0, 0, 90, 1.606, 1.194, 2.308, 9, '2024-05-11 13:40:16'),
(205, '2637247E', 'Player S', 18, 19, 0, 0, 90.48, 1.526, 0.836, 2.951, 9, '2024-05-11 13:41:30'),
(206, '1CF2C738', 'Player L', 26, 18, 0, 0, 90, 1.563, 0.914, 2.301, 1, '2024-05-11 13:45:20'),
(207, 'A15D2F1D', 'Player Z', 11, 22, 0, 0, 88, 1.203, 0.705, 2.452, 2, '2024-05-11 13:51:26'),
(208, 'FED96E85', 'Aldrifa Farizki', 78, 6, 0, 0, 66.67, 4.339, 1.281, 12.483, 3, '2024-05-15 14:32:04'),
(209, 'C32D2C40', 'Sinta Widianingrum', 30, 18, 0, 0, 100, 1.706, 1.05, 3.584, 2, '2024-05-15 14:33:27'),
(210, '10A83A0E', 'Nathan Wijaya', 38, 16, 0, 0, 76.19, 1.422, 0.97, 2.294, 0, '2024-05-16 08:45:00'),
(211, 'FED96E85', 'Aldrifa Farizki', 29, 18, 0, 0, 90, 1.603, 1.038, 3.242, 3, '2024-05-16 09:15:04'),
(212, '06036F90', 'Player K', 69, 9, 0, 0, 75, 3.129, 0.98, 6.563, 3, '2024-05-16 09:17:20'),
(213, 'E76D9A7B', 'Player A', 47, 15, 0, 0, 88.24, 1.888, 1.036, 4.992, 1, '2024-05-16 09:18:28'),
(214, '3A9A6D16', 'Group 1', 58, 12, 0, 0, 92.31, 2.522, 0.945, 10.504, 23, '2024-05-16 09:32:41'),
(215, '2AF22816', 'Group 8', 34, 17, 0, 0, 89.47, 1.623, 0.886, 2.974, 4, '2024-05-16 09:34:23'),
(216, '3A22D616', 'Group 5', 60, 12, 0, 0, 85.71, 2.726, 1.127, 10.328, 22, '2024-05-16 09:36:14'),
(217, '2AE43D16', 'Group 4', 9, 22, 0, 0, 88, 1.327, 0.438, 4.01, 20, '2024-05-16 09:37:10'),
(218, '3AB1F616', 'Group 3', 63, 11, 0, 0, 78.57, 2.645, 0.954, 13.753, 2, '2024-05-16 09:38:38'),
(219, 'D09A9A32', 'Group 6', 1, 25, 0, 0, 96.15, 1.183, 0.755, 1.781, 22, '2024-05-16 09:39:36'),
(220, 'D358F965', 'Group 7', 23, 18, 0, 0, 81.82, 1.438, 0.719, 4.161, 7, '2024-05-16 09:41:08'),
(221, '22D9BCFC', 'Group 2', 50, 14, 0, 0, 93.33, 1.941, 0.783, 4.452, 15, '2024-05-16 09:41:58'),
(222, '2AF22816', 'Group 8', 48, 15, 0, 0, 88.24, 1.732, 1.088, 4.535, 4, '2024-05-16 09:44:14'),
(223, '2AF22816', 'Group 8', 61, 12, 0, 0, 92.31, 2.457, 1.521, 3.867, 4, '2024-05-16 09:45:22'),
(224, '2AE43D16', 'Group 4', 57, 13, 0, 0, 92.86, 2.156, 0.896, 5.428, 20, '2024-05-16 09:46:38'),
(225, '2AE43D16', 'Group 4', 10, 22, 0, 0, 91.67, 1.287, 0.618, 4.848, 20, '2024-05-16 09:47:43'),
(226, 'D09A9A32', 'Group 6', 17, 19, 0, 0, 79.17, 1.366, 0.732, 3.55, 22, '2024-05-16 09:50:48'),
(227, 'D09A9A32', 'Group 6', 6, 23, 0, 0, 92, 1.252, 0.835, 3.546, 22, '2024-05-16 09:51:51'),
(228, 'D358F965', 'Group 7', 16, 19, 0, 0, 90.48, 1.519, 0.72, 7.582, 7, '2024-05-16 09:53:03'),
(229, 'D358F965', 'Group 7', 41, 15, 0, 0, 88.24, 1.868, 0.778, 3.325, 7, '2024-05-16 09:54:05'),
(230, '2AE43D16', 'Group 4', 72, 8, 0, 0, 66.67, 3.086, 0.419, 9.609, 20, '2024-05-16 09:55:42'),
(231, 'D09A9A32', 'Group 6', 15, 20, 0, 0, 90.91, 1.399, 0.834, 2.783, 22, '2024-05-16 09:57:24'),
(232, '2AE43D16', 'Group 4', 51, 14, 0, 0, 93.33, 2.001, 0.785, 5.082, 20, '2024-05-16 09:58:29'),
(233, 'D09A9A32', 'Group 6', 36, 16, 0, 0, 84.21, 1.611, 0.823, 4.554, 22, '2024-05-16 10:00:47'),
(234, '2AE43D16', 'Group 4', 4, 23, 0, 0, 79.31, 1.157, 0.615, 2.678, 20, '2024-05-16 10:02:12'),
(235, 'D09A9A32', 'Group 6', 12, 21, 0, 0, 91.3, 1.352, 0.862, 3.306, 22, '2024-05-16 10:03:10'),
(236, 'D358F965', 'Group 7', 24, 18, 0, 0, 90, 1.603, 0.734, 2.843, 7, '2024-05-16 10:07:08'),
(237, '2AF22816', 'Group 8', 44, 15, 0, 0, 71.43, 1.434, 0.873, 1.872, 4, '2024-05-16 10:08:12'),
(238, '6A59EDAF', 'Player G', 14, 20, 0, 0, 76.92, 1.208, 0.713, 3.725, 2, '2024-05-16 10:10:45'),
(239, '97918B7A', 'Player V', 56, 13, 0, 0, 86.67, 2.086, 0.486, 4.547, 9, '2024-05-16 10:11:56'),
(240, 'E76D9A7B', 'Player A', 5, 23, 0, 0, 92, 1.198, 0.808, 1.849, 1, '2024-05-16 10:13:18'),
(241, 'FED96E85', 'Aldrifa Farizki', 59, 12, 0, 0, 75, 1.745, 1.106, 4.357, 3, '2024-07-05 17:14:45'),
(242, '16D57D8D', 'Player O', 53, 14, 0, 0, 87.5, 1.811, 1.054, 2.528, 5, '2024-07-05 17:18:24'),
(243, '2637247E', 'Player S', 43, 15, 0, 0, 83.33, 1.668, 0.872, 4.616, 9, '2024-07-05 17:20:50');

--
-- Triggers `smart_wall`
--
DROP TRIGGER IF EXISTS `newest_entry_smart_wall`;
DELIMITER $$
CREATE TRIGGER `newest_entry_smart_wall` AFTER INSERT ON `smart_wall` FOR EACH ROW BEGIN
    -- Insert the newest entry into the display table
 INSERT INTO `display`.`smart_wall` (`GameID`, `ID`, `PlayerName`,`OverallRank`, `Point`, `Wrong`, `Score`,`Accuracy`,`Average`,`Fastest`,`Slowest`, `Age`,`EntryDate`)  
SELECT NEW.`GameID`,  NEW.`ID`, NEW.`PlayerName`, NEW.`OverallRank`, NEW.`Point`, NEW.`Wrong`, NEW.`Score`, NEW.`Accuracy`, NEW.`Average`,NEW.`Fastest`,NEW.`Slowest`, NEW.`Age`, NEW.`EntryDate`
    FROM `sportscience`.`smart_wall` AS u
    WHERE u.`PlayerName` = NEW.`PlayerName`
    ORDER BY u.`EntryDate` DESC
    LIMIT 1;

SET @overall_rank = 0;
UPDATE `display`.`smart_wall` AS s
JOIN (
    SELECT `Point`, `Fastest`, (@overall_rank := @overall_rank + 1) AS `OverallRank`
    FROM `sportscience`.`smart_wall`
    ORDER BY `Point` DESC, `Fastest` ASC
) AS t ON s.`Point` = t.`Point` AND s.`Fastest` = t.`Fastest`
SET s.`OverallRank` = t.`OverallRank`;

END
$$
DELIMITER ;
DROP TRIGGER IF EXISTS `smart_wall_user`;
DELIMITER $$
CREATE TRIGGER `smart_wall_user` BEFORE INSERT ON `smart_wall` FOR EACH ROW BEGIN
    DECLARE v_player_name VARCHAR(255);
    DECLARE v_age INT;

    -- Retrieve Player Name and Age from user identification
    SELECT `PlayerName`, `Age` INTO v_player_name, v_age
    FROM `user`
    WHERE ID = NEW.ID;

    -- Set the Player Name and Age for the new row
    SET NEW.`PlayerName` = v_player_name;
    SET NEW.Age = v_age;
END
$$
DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `sprint_new`
--

DROP TABLE IF EXISTS `sprint_new`;
CREATE TABLE `sprint_new` (
  `GameID` int(255) NOT NULL,
  `ID` varchar(55) NOT NULL,
  `PlayerName` varchar(255) NOT NULL,
  `OverallRank` int(11) NOT NULL,
  `Time` float NOT NULL,
  `AvgSpeed` float NOT NULL,
  `TopSpeed` float NOT NULL,
  `TopAcceleration` float NOT NULL,
  `Age` int(5) NOT NULL,
  `EntryDate` datetime NOT NULL DEFAULT current_timestamp(),
  `Source` varchar(5) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `sprint_new`
--

INSERT INTO `sprint_new` (`GameID`, `ID`, `PlayerName`, `OverallRank`, `Time`, `AvgSpeed`, `TopSpeed`, `TopAcceleration`, `Age`, `EntryDate`, `Source`) VALUES
(411, '10A83A0E', 'Steven', 7, 13.92, 5.18, 5.48, 0.02, 0, '2024-02-08 11:02:43', 'Right'),
(412, '22D9BCFC', 'Tiara', 9, 18.83, 3.9, 4.73, 0.05, 15, '2024-02-08 12:05:19', 'Left'),
(413, '10A83A0E', 'Steven', 8, 18.67, 4.09, 5.81, 0.09, 0, '2024-02-08 12:05:20', 'Right'),
(414, '10A83A0E', 'Steven', 2, 2.86, 26.43, 31.3, 2.1, 0, '2024-02-08 12:06:46', 'Right'),
(415, '3A9A6D16', 'Muzakki', 4, 3.54, 21.39, 25.17, 1.53, 23, '2024-02-08 12:06:47', 'Left'),
(416, '10A83A0E', 'Steven', 1, 2.86, 26.32, 31.36, 2, 0, '2024-02-08 12:08:09', 'Right'),
(417, '3A9A6D16', 'Muzakki', 6, 3.69, 19.68, 21.45, 0.67, 23, '2024-02-08 12:08:10', 'Left'),
(418, '3A9A6D16', 'Muzakki', 3, 3.32, 22.64, 26.63, 1.64, 23, '2024-02-08 12:09:50', 'Left'),
(419, '10A83A0E', 'Steven', 5, 3.58, 20.84, 24.83, 1.09, 0, '2024-02-08 12:09:50', 'Right');

--
-- Triggers `sprint_new`
--
DROP TRIGGER IF EXISTS `newest_entry_sprint_new`;
DELIMITER $$
CREATE TRIGGER `newest_entry_sprint_new` AFTER INSERT ON `sprint_new` FOR EACH ROW BEGIN
    -- Insert the newest entry into the display table
    INSERT INTO `display`.`sprint_new` (`PlayerName`, `Time`, `ID`, `EntryDate`, `Age`, `GameID`, `OverallRank`, `AvgSpeed`,`TopSpeed`, `TopAcceleration`, `Source`)
    SELECT NEW.`PlayerName`, NEW.`Time`, NEW.`ID`, NEW.`EntryDate`, NEW.`Age`, NEW.`GameID`, NEW.`OverallRank`, NEW.`AvgSpeed`, NEW.`TopSpeed`, NEW.`TopAcceleration`, NEW.`Source`
    FROM `sportscience`.`sprint_new` AS u
    WHERE u.`PlayerName` = NEW.`PlayerName`
    ORDER BY u.`EntryDate` DESC
    LIMIT 1;

     SET @overall_rank = 0;
     UPDATE `display`.`sprint_new` AS s
     JOIN (
        SELECT `Time`, `EntryDate`, (@overall_rank := @overall_rank + 1) AS `OverallRank`
         FROM `sportscience`.`sprint_new`
         ORDER BY `Time` ASC, `EntryDate` DESC
    ) AS t ON s.`Time` = t.`Time` AND s.`EntryDate` = t.`EntryDate`
    SET s.`OverallRank` = t.`OverallRank`;
END
$$
DELIMITER ;
DROP TRIGGER IF EXISTS `sprint_new_user`;
DELIMITER $$
CREATE TRIGGER `sprint_new_user` BEFORE INSERT ON `sprint_new` FOR EACH ROW BEGIN
    DECLARE v_player_name VARCHAR(255);
    DECLARE v_age INT;

    -- Retrieve Player Name and Age from user identification
    SELECT `PlayerName`, `Age` INTO v_player_name, v_age
    FROM `user`
    WHERE ID = NEW.ID;

    -- Set the Player Name and Age for the new row
    SET NEW.`PlayerName` = v_player_name;
    SET NEW.Age = v_age;
END
$$
DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `user`
--

DROP TABLE IF EXISTS `user`;
CREATE TABLE `user` (
  `ID` varchar(55) NOT NULL,
  `PlayerName` varchar(255) NOT NULL,
  `DoB` date NOT NULL,
  `Email` varchar(255) NOT NULL,
  `Whatsapp` varchar(20) NOT NULL,
  `Age` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `user`
--

INSERT INTO `user` (`ID`, `PlayerName`, `DoB`, `Email`, `Whatsapp`, `Age`) VALUES
('06036f90', 'Player K', '2000-01-01', '-', '0', 3),
('0c131239', 'Player R', '2003-01-01', '-', '0', 8),
('10A83A0E', 'Nathan Wijaya', '2023-01-01', '-', '0', 0),
('16d57d8d', 'Player O', '2000-12-14', '-', '0', 5),
('19bd0b40', 'Employee C', '2001-09-08', '-', '0', 19),
('1cf2c738', 'Player L', '2000-01-02', '-', '0', 1),
('22D9BCFC', 'Group 2', '2008-04-01', '-', '0', 15),
('2637247e', 'Player S', '2004-03-01', '-', '0', 9),
('2AE43D16', 'Group 4', '2003-02-03', '-', '0', 20),
('2af22816', 'Group 8', '2000-04-05', '-', '0', 4),
('2c649534', 'Janice', '2003-08-07', '-', '0', 20),
('3A22D616', 'Group 5', '2000-12-08', '-', '0', 22),
('3A9A6D16', 'Group 1', '2000-10-10', '-', '0', 23),
('3AB1F616', 'Group 3', '2021-01-05', '-', '0', 2),
('47C6AF7A', 'Player B', '2001-01-01', '-', '0', 15),
('4a3b4016', 'FET Tombak', '2021-01-01', '-', '0', 0),
('4c729b34', 'Wahyu', '2000-01-01', '-', '0', 20),
('56FD7F8D', 'Player C', '2001-01-01', '-', '0', 23),
('5a200b40', 'Employee B', '2001-01-01', '-', '0', 20),
('5AE10FB1', 'Player W', '2021-01-01', '-', '0', 2),
('5ccd8d34', 'Employee D', '2001-02-03', '-', '0', 20),
('6A59EDAF', 'Player G', '2021-02-05', '-', '0', 2),
('729DBEFC', 'Player G', '2020-12-12', '-', '0', 2),
('76F27E8D', 'Player Y', '2001-04-05', '-', '0', 22),
('81ee551d', 'Player M', '2000-11-03', '-', '0', 2),
('91EEFA1D', 'Player U ', '2001-03-02', '-', '0', 22),
('97918B7A', 'Player V', '2014-11-01', '-', '0', 9),
('9eaf0b40', 'Aldrifa', '2021-01-01', '-', '0', 23),
('A15D2F1D', 'Player Z', '2021-01-04', '-', '0', 2),
('a191321d', 'Player N', '2000-01-01', '-', '0', 4),
('B2C70B40', 'ME Djati', '2012-01-04', '-', '0', 11),
('b31f0b40', 'Employee E', '2001-06-07', '-', '0', 22),
('B69C9D2B', 'FAS Ilham', '2000-12-12', '-', '0', 22),
('BA1C26B0', 'Player X', '2001-11-14', '-', '0', 22),
('c32d2c40', 'Sinta Widianingrum', '2000-12-21', '-', '0', 2),
('D09A9A32', 'Group 6', '2000-12-14', '-', '0', 22),
('d358f965', 'Group 7', '2000-04-05', '-', '0', 7),
('D6776990', 'Player H', '2021-08-04', '-', '0', 2),
('da73fdaf', 'Player P', '2000-01-04', '-', '0', 6),
('dc0ed234', 'Annisa', '2001-04-04', '-', '0', 22),
('E76D9A7B', 'Player A', '2022-02-01', '-', '0', 1),
('ec62d734', 'Fitri', '2000-02-02', '-', '-', 23),
('fae10cb0', 'Player T', '2006-01-04', '-', '0', 10),
('fed96e85', 'Aldrifa Farizki', '2000-04-05', '-', '0', 3);

--
-- Triggers `user`
--
DROP TRIGGER IF EXISTS `update_age`;
DELIMITER $$
CREATE TRIGGER `update_age` BEFORE INSERT ON `user` FOR EACH ROW BEGIN
    -- Calculate the Age based on Date of Birth
    SET @age = TIMESTAMPDIFF(YEAR, NEW.`DoB`, CURDATE());
    -- Insert the new row with the calculated Age
    SET NEW.Age = @age;
END
$$
DELIMITER ;

--
-- Indexes for dumped tables
--

--
-- Indexes for table `agility pole run`
--
ALTER TABLE `agility pole run`
  ADD PRIMARY KEY (`GameID`),
  ADD KEY `ID` (`ID`);

--
-- Indexes for table `reaction_training`
--
ALTER TABLE `reaction_training`
  ADD PRIMARY KEY (`GameID`),
  ADD KEY `ID` (`ID`);

--
-- Indexes for table `shooting_accuracy`
--
ALTER TABLE `shooting_accuracy`
  ADD PRIMARY KEY (`GameID`),
  ADD KEY `ID` (`ID`);

--
-- Indexes for table `smart_wall`
--
ALTER TABLE `smart_wall`
  ADD PRIMARY KEY (`GameID`),
  ADD KEY `ID` (`ID`);

--
-- Indexes for table `sprint_new`
--
ALTER TABLE `sprint_new`
  ADD PRIMARY KEY (`GameID`),
  ADD KEY `ID` (`ID`);

--
-- Indexes for table `user`
--
ALTER TABLE `user`
  ADD PRIMARY KEY (`ID`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `agility pole run`
--
ALTER TABLE `agility pole run`
  MODIFY `GameID` int(255) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=134;

--
-- AUTO_INCREMENT for table `reaction_training`
--
ALTER TABLE `reaction_training`
  MODIFY `GameID` int(255) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=318;

--
-- AUTO_INCREMENT for table `shooting_accuracy`
--
ALTER TABLE `shooting_accuracy`
  MODIFY `GameID` int(255) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=41;

--
-- AUTO_INCREMENT for table `smart_wall`
--
ALTER TABLE `smart_wall`
  MODIFY `GameID` int(255) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=244;

--
-- AUTO_INCREMENT for table `sprint_new`
--
ALTER TABLE `sprint_new`
  MODIFY `GameID` int(255) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=420;

--
-- Constraints for dumped tables
--

--
-- Constraints for table `agility pole run`
--
ALTER TABLE `agility pole run`
  ADD CONSTRAINT `fk_agility_pole_run_user` FOREIGN KEY (`ID`) REFERENCES `user` (`ID`) ON DELETE NO ACTION ON UPDATE NO ACTION;

--
-- Constraints for table `reaction_training`
--
ALTER TABLE `reaction_training`
  ADD CONSTRAINT `fk_reaction_wall_user` FOREIGN KEY (`ID`) REFERENCES `user` (`ID`) ON DELETE NO ACTION ON UPDATE NO ACTION;

--
-- Constraints for table `shooting_accuracy`
--
ALTER TABLE `shooting_accuracy`
  ADD CONSTRAINT `fk_shooting_accuracy_user` FOREIGN KEY (`ID`) REFERENCES `user` (`ID`) ON DELETE NO ACTION ON UPDATE NO ACTION;

DELIMITER $$
--
-- Events
--
DROP EVENT IF EXISTS `rank_reaction_training`$$
CREATE DEFINER=`root`@`localhost` EVENT `rank_reaction_training` ON SCHEDULE EVERY 1 SECOND STARTS '2023-08-16 16:03:58' ON COMPLETION PRESERVE ENABLE DO BEGIN
    -- Update Rank in agility
    UPDATE `sportscience`.`reaction_training` AS s
    JOIN (
        SELECT
            `Fastest`,
            `Point`,
        ROW_NUMBER() OVER (ORDER BY `Point` DESC, `Fastest` ASC) AS OverallRank
        FROM
            `sportscience`.`reaction_training`
    ) AS t ON s.`Fastest` = t.`Fastest` AND s.`Point` = t.`Point`
    SET s.`OverallRank` = t.`OverallRank`;
END$$

DROP EVENT IF EXISTS `rank_agility_pole_run`$$
CREATE DEFINER=`root`@`localhost` EVENT `rank_agility_pole_run` ON SCHEDULE EVERY 1 SECOND STARTS '2023-08-16 16:03:58' ON COMPLETION PRESERVE ENABLE DO BEGIN
    -- Update Rank in agility
    UPDATE `sportscience`.`agility pole run` AS s
    JOIN (
        SELECT
            `EntryDate`,
            `FinalTime`,
            ROW_NUMBER() OVER (ORDER BY `FinalTime`, `EntryDate` DESC) AS OverallRank
        FROM
            `sportscience`.`agility pole run`
    ) AS t ON s.`EntryDate` = t.`EntryDate` AND s.`FinalTime` = t.`FinalTime`
    SET s.`OverallRank` = t.`OverallRank`;
END$$

DROP EVENT IF EXISTS `rank_sprint_new`$$
CREATE DEFINER=`root`@`localhost` EVENT `rank_sprint_new` ON SCHEDULE EVERY 1 SECOND STARTS '2023-08-16 16:03:58' ON COMPLETION PRESERVE ENABLE DO BEGIN
    -- Update Rank in sprint
    UPDATE `sportscience`.`sprint_new` AS s
    JOIN (
        SELECT
            `EntryDate`,
            `Time`,
            ROW_NUMBER() OVER (ORDER BY `Time`, `EntryDate` DESC) AS OverallRank
        FROM
            `sportscience`.`sprint_new`
    ) AS t ON s.`EntryDate` = t.`EntryDate` AND s.`Time` = t.`Time`
    SET s.`OverallRank` = t.`OverallRank`;
END$$

DROP EVENT IF EXISTS `rank_smart_wall`$$
CREATE DEFINER=`root`@`localhost` EVENT `rank_smart_wall` ON SCHEDULE EVERY 1 SECOND STARTS '2023-08-16 16:03:58' ON COMPLETION PRESERVE ENABLE DO BEGIN
    -- Update Rank in agility
    UPDATE `sportscience`.`smart_wall` AS s
    JOIN (
        SELECT
            `Fastest`,
            `Point`,
        ROW_NUMBER() OVER (ORDER BY `Point` DESC, `Fastest` ASC) AS OverallRank
        FROM
            `sportscience`.`smart_wall`
    ) AS t ON s.`Fastest` = t.`Fastest` AND s.`Point` = t.`Point`
    SET s.`OverallRank` = t.`OverallRank`;
END$$

DROP EVENT IF EXISTS `rank_shooting_accuracy`$$
CREATE DEFINER=`root`@`localhost` EVENT `rank_shooting_accuracy` ON SCHEDULE EVERY 1 SECOND STARTS '2023-08-16 16:03:58' ON COMPLETION PRESERVE ENABLE DO BEGIN
    -- Update Rank in reaction_training
    UPDATE `sportscience`.`shooting_accuracy` AS s
    JOIN (
        SELECT
            `Accuracy`,
            `Goal`,
        ROW_NUMBER() OVER (ORDER BY `Goal` DESC, `Accuracy` DESC) AS OverallRank
        FROM
            `sportscience`.`shooting_accuracy`
    ) AS t ON s.`Accuracy` = t.`Accuracy` AND s.`Goal` = t.`Goal`
    SET s.`OverallRank` = t.`OverallRank`;
END$$

DELIMITER ;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
