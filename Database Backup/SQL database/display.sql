-- phpMyAdmin SQL Dump
-- version 5.2.1
-- https://www.phpmyadmin.net/
--
-- Host: 127.0.0.1
-- Generation Time: Jul 09, 2024 at 12:29 PM
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
-- Database: `display`
--
CREATE DATABASE IF NOT EXISTS `display` DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;
USE `display`;

DELIMITER $$
--
-- Procedures
--
DROP PROCEDURE IF EXISTS `reset_agility`$$
CREATE DEFINER=`root`@`localhost` PROCEDURE `reset_agility` ()   BEGIN
DELETE FROM `agility`;
DELETE FROM `agility_main`;
DELETE FROM `agility_recent`;
END$$

DROP PROCEDURE IF EXISTS `reset_reaction`$$
CREATE DEFINER=`root`@`localhost` PROCEDURE `reset_reaction` ()   BEGIN
DELETE FROM `reaction`;
DELETE FROM `reaction_main`;
DELETE FROM `reaction_recent`;
END$$

DROP PROCEDURE IF EXISTS `reset_shooting`$$
CREATE DEFINER=`root`@`localhost` PROCEDURE `reset_shooting` ()   BEGIN
DELETE FROM `shooting`;
DELETE FROM `shooting_main`;
DELETE FROM `shooting_recent`;
END$$

DROP PROCEDURE IF EXISTS `reset_smart_wall`$$
CREATE DEFINER=`root`@`localhost` PROCEDURE `reset_smart_wall` ()   BEGIN
DELETE FROM `smart_wall`;
DELETE FROM `smart_wall_main`;
DELETE FROM `smart_wall_recent`;
END$$

DROP PROCEDURE IF EXISTS `reset_sprint_new`$$
CREATE DEFINER=`root`@`localhost` PROCEDURE `reset_sprint_new` ()   BEGIN
DELETE FROM `sprint_new`;
DELETE FROM `sprint_new_main`;
DELETE FROM `sprint_new_left`;
DELETE FROM `sprint_new_right`;
END$$

DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `agility`
--

DROP TABLE IF EXISTS `agility`;
CREATE TABLE `agility` (
  `GameID` int(255) NOT NULL,
  `ID` varchar(55) NOT NULL,
  `OverallRank` int(11) NOT NULL,
  `Rank` int(11) NOT NULL,
  `PlayerName` varchar(255) NOT NULL,
  `Time` float NOT NULL,
  `Speed` float NOT NULL,
  `Penalty` int(11) NOT NULL,
  `FinalTime` float NOT NULL,
  `Age` int(5) NOT NULL,
  `EntryDate` datetime NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `agility`
--

INSERT INTO `agility` (`GameID`, `ID`, `OverallRank`, `Rank`, `PlayerName`, `Time`, `Speed`, `Penalty`, `FinalTime`, `Age`, `EntryDate`) VALUES
(130, '22D9BCFC', 1, 1, 'Tiara', 8.1, 2.47, 5, 18.1, 15, '2024-02-08 10:51:53'),
(131, '22D9BCFC', 2, 2, 'Tiara', 8.54, 2.34, 5, 18.54, 15, '2024-02-08 11:28:14'),
(132, '9EAF0B40', 3, 3, 'Aldrifa', 13.54, 1.48, 5, 23.54, 23, '2024-02-08 12:10:57'),
(133, '9EAF0B40', 4, 4, 'Aldrifa', 16.24, 1.23, 5, 26.24, 23, '2024-02-08 12:12:42');

--
-- Triggers `agility`
--
DROP TRIGGER IF EXISTS `refresh_main_agility`;
DELIMITER $$
CREATE TRIGGER `refresh_main_agility` AFTER INSERT ON `agility` FOR EACH ROW BEGIN
    -- Delete existing entries from the display table
    DELETE FROM `display`.`agility_main`;

    -- Insert new entries into the display table based on the lowest "Time" for each player name and oldest entry date
    INSERT INTO `display`.`agility_main` (`PlayerName`, `Time`, `Speed`,`Penalty`, `FinalTime`,`ID`, `EntryDate`, `Age`, `GameID`, `OverallRank`)
    SELECT
        a.`PlayerName`,
        a.`Time`,
        a.`Speed`,
        a.`Penalty`,
        a.`FinalTime`,
        a.`ID`,
        a.`EntryDate`,
        a.`Age`,
        a.`GameID`,
        a.`OverallRank`
    FROM (
        SELECT `PlayerName`, MIN(`FinalTime`) AS minFinalTime
        FROM `display`.`agility`
        GROUP BY `PlayerName`
    ) AS minFinalTimeSubquery
    JOIN `display`.`agility` AS a
    ON a.`PlayerName` = minFinalTimeSubquery.`PlayerName` AND a.`FinalTime` = minFinalTimeSubquery.`minFinalTime`
    AND a.`EntryDate` = (
        SELECT MIN(`EntryDate`)
        FROM `display`.`agility` b
        WHERE b.`PlayerName` = a.`PlayerName` AND b.`FinalTime` = a.`FinalTime`
    );

    SET @rank = 0;

    -- Update the rank within "agility_main" based on the lowest "FinalTime" for each unique "PlayerName"
-- Update the rank in "agility_main" and consider only the row with the lowest "FinalTime" for the same "ID" from "agility"
-- Update Rank for the "agility_main" table only
UPDATE display.agility_main a
SET a.Rank = (
  SELECT COUNT(*) + 1
  FROM display.agility_main b
  WHERE (b.FinalTime < a.FinalTime OR (b.FinalTime = a.FinalTime AND b.EntryDate < a.EntryDate))
);

-- Set Rank to 1 for the row with the lowest FinalTime and oldest EntryDate
UPDATE display.agility_main
SET Rank = 1
WHERE GameID = (
  SELECT GameID
  FROM display.agility_main
  ORDER BY FinalTime, EntryDate
  LIMIT 1
);



END
$$
DELIMITER ;
DROP TRIGGER IF EXISTS `update_agility_recent`;
DELIMITER $$
CREATE TRIGGER `update_agility_recent` AFTER INSERT ON `agility` FOR EACH ROW BEGIN
    -- Delete the previous row in agility_recent
    DELETE FROM display.agility_recent;

    -- Insert the latest row from agility into agility_recent
    INSERT INTO display.agility_recent (`PlayerName`, `Time`,`Speed`,`Penalty`, `FinalTime`,`ID`, `EntryDate`, `Age`, `GameID`, `Rank`, `OverallRank`)
    SELECT NEW.`PlayerName`, NEW.`Time`, NEW.`Speed`, NEW.`Penalty`, NEW.`FinalTime`, NEW.`ID`, NEW.`EntryDate`, NEW.`Age`, NEW.`GameID`, NEW.`Rank`, agility.`OverallRank`
    FROM display.agility
    WHERE NEW.`ID` = agility.`ID`
    ORDER BY NEW.`EntryDate` DESC
    LIMIT 1;
    
        -- Update the Rank and OverallRank columns in agility_recent
    UPDATE display.agility_recent AS ar
    JOIN display.agility AS a ON ar.GameID = a.GameID
    SET ar.`Rank` = a.`Rank`,
        ar.`OverallRank` = a.`OverallRank`;
END
$$
DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `agility_main`
--

DROP TABLE IF EXISTS `agility_main`;
CREATE TABLE `agility_main` (
  `GameID` int(255) NOT NULL,
  `ID` varchar(55) NOT NULL,
  `OverallRank` int(11) NOT NULL,
  `Rank` int(11) NOT NULL,
  `PlayerName` varchar(255) NOT NULL,
  `Time` float NOT NULL,
  `Speed` float NOT NULL,
  `Penalty` int(11) NOT NULL,
  `FinalTime` float NOT NULL,
  `Age` int(5) NOT NULL,
  `EntryDate` datetime NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `agility_main`
--

INSERT INTO `agility_main` (`GameID`, `ID`, `OverallRank`, `Rank`, `PlayerName`, `Time`, `Speed`, `Penalty`, `FinalTime`, `Age`, `EntryDate`) VALUES
(130, '22D9BCFC', 1, 1, 'Tiara', 8.1, 2.47, 5, 18.1, 15, '2024-02-08 10:51:53'),
(132, '9EAF0B40', 3, 2, 'Aldrifa', 13.54, 1.48, 5, 23.54, 23, '2024-02-08 12:10:57');

-- --------------------------------------------------------

--
-- Table structure for table `agility_recent`
--

DROP TABLE IF EXISTS `agility_recent`;
CREATE TABLE `agility_recent` (
  `GameID` int(255) NOT NULL,
  `ID` varchar(55) NOT NULL,
  `OverallRank` int(11) NOT NULL,
  `Rank` int(11) NOT NULL,
  `PlayerName` varchar(255) NOT NULL,
  `Time` float NOT NULL,
  `Speed` float NOT NULL,
  `Penalty` int(11) NOT NULL,
  `FinalTime` float NOT NULL,
  `Age` int(5) NOT NULL,
  `EntryDate` datetime NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `agility_recent`
--

INSERT INTO `agility_recent` (`GameID`, `ID`, `OverallRank`, `Rank`, `PlayerName`, `Time`, `Speed`, `Penalty`, `FinalTime`, `Age`, `EntryDate`) VALUES
(133, '9EAF0B40', 4, 4, 'Aldrifa', 16.24, 1.23, 5, 26.24, 23, '2024-02-08 12:12:42');

-- --------------------------------------------------------

--
-- Table structure for table `reaction`
--

DROP TABLE IF EXISTS `reaction`;
CREATE TABLE `reaction` (
  `GameID` int(255) NOT NULL,
  `ID` varchar(55) NOT NULL,
  `OverallRank` int(11) NOT NULL,
  `Rank` int(11) NOT NULL,
  `PlayerName` varchar(255) NOT NULL,
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
-- Dumping data for table `reaction`
--

INSERT INTO `reaction` (`GameID`, `ID`, `OverallRank`, `Rank`, `PlayerName`, `Point`, `TotalPoint`, `Wrong`, `Score`, `Accuracy`, `Average`, `Fastest`, `Slowest`, `Age`, `EntryDate`) VALUES
(305, '9EAF0B40', 1, 1, 'Aldrifa', 41, 54, 0, 0, 75.93, 0.447, 0.01, 0.606, 23, '2024-03-02 11:58:53'),
(306, '2637247E', 36, 9, 'Player S', 19, 31, 0, 0, 61.29, 0.6, 0.009, 1.478, 9, '2024-03-02 11:59:47'),
(307, 'DA73FDAF', 12, 5, 'Player P', 34, 42, 0, 0, 80.95, 0.462, 0.031, 0.937, 6, '2024-03-02 12:02:11'),
(308, '9EAF0B40', 10, 4, 'Aldrifa', 34, 40, 0, 0, 85, 0.549, 0.01, 0.862, 23, '2024-03-02 12:42:43'),
(309, '0C131239', 45, 11, 'Player R', 13, 20, 0, 0, 65, 0.821, 0.011, 1.428, 8, '2024-03-02 12:43:51'),
(310, '10A83A0E', 50, 12, 'Nathan Wijaya', 6, 2, 0, 0, 2.97, 606820, 0.009, 3640920, 0, '2024-07-09 11:01:19'),
(311, 'A15D2F1D', 13, 6, 'Player Z', 33, 68, 0, 0, 68.75, 0.541, 0.01, 1.822, 2, '2024-07-09 11:02:47'),
(312, 'C32D2C40', 27, 8, 'Sinta Widianingrum', 25, 51, 0, 0, 51.02, 0.702, 0.009, 1.363, 2, '2024-07-09 11:05:44'),
(313, 'D6776990', 4, 2, 'Player H', 38, 67, 0, 0, 67.86, 0.452, 0.009, 0.667, 2, '2024-07-09 11:06:57'),
(314, '6A59EDAF', 43, 10, 'Player G', 14, 100, 0, 0, 100, 1.176, 0.71, 2.028, 2, '2024-07-09 11:09:03'),
(315, '91EEFA1D', 23, 7, 'Player U ', 27, 81, 0, 0, 81.82, 0.605, 0.01, 1.402, 22, '2024-07-09 11:09:58'),
(316, '91EEFA1D', 51, 13, 'Player U ', 4, 100, 0, 0, 100, 5.691, 2.418, 10.126, 22, '2024-07-09 11:10:46'),
(317, 'FED96E85', 9, 3, 'Aldrifa Farizki', 35, 72, 0, 0, 72.92, 0.487, 0.01, 1.106, 3, '2024-07-09 11:11:19');

--
-- Triggers `reaction`
--
DROP TRIGGER IF EXISTS `refresh_reaction_main`;
DELIMITER $$
CREATE TRIGGER `refresh_reaction_main` AFTER INSERT ON `reaction` FOR EACH ROW BEGIN
    -- Delete existing entries from the display table
    DELETE FROM `display`.`reaction_main`;

    -- Insert new entries into the display table based on the highest "Score" for each player name and oldest entry date
    INSERT INTO `display`.`reaction_main` (`GameID`, `ID`, `PlayerName`, `OverallRank`, `Point`,`TotalPoint`, `Wrong`, `Score`, `Accuracy`, `Average`,`Fastest`,`Slowest`, `Age`, `EntryDate`)  
    SELECT a.`GameID`, a.`ID`, a.`PlayerName`, a.`OverallRank`, a.`Point`, a.`TotalPoint`, a.`Wrong`, a.`Score`, a.`Accuracy`, a.`Average`, a.`Fastest`,a.`Slowest`, a.`Age`, a.`EntryDate`
    FROM (
        SELECT `PlayerName`, MAX(`Point`) AS maxPoint
        FROM `display`.`reaction`
        GROUP BY `PlayerName`
    ) AS maxPointSubquery
    JOIN `display`.`reaction` AS a
    ON a.`PlayerName` = maxPointSubquery.`PlayerName` AND a.`Point` = maxPointSubquery.`maxPoint`
    AND a.`EntryDate` = (
        SELECT MIN(`EntryDate`)
        FROM `display`.`reaction` b
        WHERE b.`PlayerName` = a.`PlayerName` AND b.`Point` = a.`Point`
    );

    SET @rank = 0;

    -- Update the rank within "reaction_main" based on the highest Point for each unique "PlayerName"
    UPDATE `display`.`reaction_main` a
    SET a.Rank = (SELECT COUNT(*) + 1
        FROM `display`.`reaction_main` b
        WHERE (b.Point > a.Point OR (b.Point = a.Point AND b.Fastest < a.Fastest))
    );

    -- Set Rank to 1 for the row with the highest Point and fastest reaction time
UPDATE display.reaction_main
SET Rank = 1
WHERE GameID = (
  SELECT GameID
  FROM display.reaction_main
  ORDER BY `Point` DESC, `Fastest` ASC
  LIMIT 1
);

END
$$
DELIMITER ;
DROP TRIGGER IF EXISTS `update_reaction_recent`;
DELIMITER $$
CREATE TRIGGER `update_reaction_recent` AFTER INSERT ON `reaction` FOR EACH ROW BEGIN
    -- Delete the previous row in reaction_recent
    DELETE FROM display.reaction_recent;

    -- Insert the latest row from reaction into reaction_recent

 INSERT INTO `display`.`reaction_recent` (`GameID`, `ID`, `PlayerName`,`OverallRank`, `Point`,`TotalPoint`, `Wrong`,`Score`,`Accuracy`,`Average`,`Fastest`,`Slowest`, `Age`,`EntryDate`)  
SELECT NEW.`GameID`,  NEW.`ID`, NEW.`PlayerName`, NEW.`OverallRank`, NEW.`Point`, NEW.`TotalPoint`, NEW.`Wrong`, NEW.`Score`, NEW.`Accuracy`, NEW.`Average`,  NEW.`Fastest`,  NEW.`Slowest`, NEW.`Age`, NEW.`EntryDate`

    FROM display.reaction
    WHERE NEW.`ID` = reaction.`ID`
    ORDER BY NEW.`EntryDate` DESC
    LIMIT 1;
    
        -- Update the Rank and OverallRank columns in reaction_recent
    UPDATE display.reaction_recent AS ar
    JOIN display.reaction AS a ON ar.GameID = a.GameID
    SET ar.`Rank` = a.`Rank`,
        ar.`OverallRank` = a.`OverallRank`;
END
$$
DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `reaction_main`
--

DROP TABLE IF EXISTS `reaction_main`;
CREATE TABLE `reaction_main` (
  `GameID` int(255) NOT NULL,
  `ID` varchar(55) NOT NULL,
  `OverallRank` int(11) NOT NULL,
  `Rank` int(11) NOT NULL,
  `PlayerName` varchar(255) NOT NULL,
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
-- Dumping data for table `reaction_main`
--

INSERT INTO `reaction_main` (`GameID`, `ID`, `OverallRank`, `Rank`, `PlayerName`, `Point`, `TotalPoint`, `Wrong`, `Score`, `Accuracy`, `Average`, `Fastest`, `Slowest`, `Age`, `EntryDate`) VALUES
(305, '9EAF0B40', 1, 1, 'Aldrifa', 41, 54, 0, 0, 75.93, 0.447, 0.01, 0.606, 23, '2024-03-02 11:58:53'),
(306, '2637247E', 36, 8, 'Player S', 19, 31, 0, 0, 61.29, 0.6, 0.009, 1.478, 9, '2024-03-02 11:59:47'),
(307, 'DA73FDAF', 12, 4, 'Player P', 34, 42, 0, 0, 80.95, 0.462, 0.031, 0.937, 6, '2024-03-02 12:02:11'),
(309, '0C131239', 45, 10, 'Player R', 13, 20, 0, 0, 65, 0.821, 0.011, 1.428, 8, '2024-03-02 12:43:51'),
(310, '10A83A0E', 50, 11, 'Nathan Wijaya', 6, 2, 0, 0, 2.97, 606820, 0.009, 3640920, 0, '2024-07-09 11:01:19'),
(311, 'A15D2F1D', 13, 5, 'Player Z', 33, 68, 0, 0, 68.75, 0.541, 0.01, 1.822, 2, '2024-07-09 11:02:47'),
(312, 'C32D2C40', 27, 7, 'Sinta Widianingrum', 25, 51, 0, 0, 51.02, 0.702, 0.009, 1.363, 2, '2024-07-09 11:05:44'),
(313, 'D6776990', 4, 2, 'Player H', 38, 67, 0, 0, 67.86, 0.452, 0.009, 0.667, 2, '2024-07-09 11:06:57'),
(314, '6A59EDAF', 43, 9, 'Player G', 14, 100, 0, 0, 100, 1.176, 0.71, 2.028, 2, '2024-07-09 11:09:03'),
(315, '91EEFA1D', 23, 6, 'Player U ', 27, 81, 0, 0, 81.82, 0.605, 0.01, 1.402, 22, '2024-07-09 11:09:58'),
(317, 'FED96E85', 9, 3, 'Aldrifa Farizki', 35, 72, 0, 0, 72.92, 0.487, 0.01, 1.106, 3, '2024-07-09 11:11:19');

-- --------------------------------------------------------

--
-- Table structure for table `reaction_recent`
--

DROP TABLE IF EXISTS `reaction_recent`;
CREATE TABLE `reaction_recent` (
  `GameID` int(255) NOT NULL,
  `ID` varchar(55) NOT NULL,
  `OverallRank` int(11) NOT NULL,
  `Rank` int(11) NOT NULL,
  `PlayerName` varchar(255) NOT NULL,
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
-- Dumping data for table `reaction_recent`
--

INSERT INTO `reaction_recent` (`GameID`, `ID`, `OverallRank`, `Rank`, `PlayerName`, `Point`, `TotalPoint`, `Wrong`, `Score`, `Accuracy`, `Average`, `Fastest`, `Slowest`, `Age`, `EntryDate`) VALUES
(317, 'FED96E85', 9, 3, 'Aldrifa Farizki', 35, 72, 0, 0, 72.92, 0.487, 0.01, 1.106, 3, '2024-07-09 11:11:19');

-- --------------------------------------------------------

--
-- Table structure for table `shooting`
--

DROP TABLE IF EXISTS `shooting`;
CREATE TABLE `shooting` (
  `GameID` int(255) NOT NULL,
  `ID` varchar(55) NOT NULL,
  `OverallRank` int(11) NOT NULL,
  `Rank` int(11) NOT NULL,
  `PlayerName` varchar(255) NOT NULL,
  `Shoot` int(11) NOT NULL,
  `Goal` int(11) NOT NULL,
  `Miss` int(11) NOT NULL,
  `Accuracy` float NOT NULL,
  `Age` int(5) NOT NULL,
  `EntryDate` datetime NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `shooting`
--

INSERT INTO `shooting` (`GameID`, `ID`, `OverallRank`, `Rank`, `PlayerName`, `Shoot`, `Goal`, `Miss`, `Accuracy`, `Age`, `EntryDate`) VALUES
(37, 'C32D2C40', 3, 3, 'Sinta', 2, 1, 1, 50, 2, '2024-02-08 10:35:33'),
(38, 'C32D2C40', 2, 2, 'Sinta', 9, 7, 2, 77.78, 2, '2024-02-08 10:36:17'),
(39, '3A9A6D16', 4, 4, 'Muzakki', 1, 0, 1, 0, 23, '2024-02-08 11:02:55'),
(40, '10A83A0E', 1, 1, 'Steven', 11, 8, 3, 72.73, 0, '2024-02-08 11:03:38');

--
-- Triggers `shooting`
--
DROP TRIGGER IF EXISTS `refresh_shooting_main`;
DELIMITER $$
CREATE TRIGGER `refresh_shooting_main` AFTER INSERT ON `shooting` FOR EACH ROW BEGIN
    -- Delete existing entries from the shooting_main table
    DELETE FROM `display`.`shooting_main`;

    -- Insert new entries into the display table based on the highest "Accuracy" for each player name and oldest entry date
    INSERT INTO `display`.`shooting_main` (`GameID`, `ID`, `PlayerName`, `OverallRank`, `Shoot`, `Goal`, `Miss`,`Accuracy`, `Age`, `EntryDate`)
    SELECT a.`GameID`, a.`ID`, a.`PlayerName`, a. `OverallRank`, a.`Shoot`, a.`Goal`, a.`Miss`, a.`Accuracy`, a.`Age`, a.`EntryDate`
    FROM (
        SELECT `PlayerName`, MAX(`Goal`) AS maxGoal
        FROM `display`.`shooting`
        GROUP BY `PlayerName`
    ) AS maxGoalSubquery
    JOIN `display`.`shooting` AS a
    ON a.`PlayerName` = maxGoalSubquery.`PlayerName` AND a.`Goal` = maxGoalSubquery.`maxGoal`
    AND a.`Accuracy` = (
        SELECT MAX(`Accuracy`)
        FROM `display`.`shooting` b
        WHERE b.`PlayerName` = a.`PlayerName` AND b.`Goal` = a.`Goal`
    );

    SET @rank = 0;

    -- Update the rank within "shooting_main" based on the highest Goal for each unique PlayerName
    UPDATE `display`.`shooting_main` a
    SET a.Rank = (
        SELECT COUNT(*) + 1
        FROM `display`.`shooting_main` b
        WHERE (b.Goal > a.Goal OR (b.Goal = a.Goal AND b.Accuracy > a.Accuracy))
    );

        -- Set Rank to 1 for the row with the highest Point and fastest reaction time
    UPDATE `display`.`shooting_main`
    SET Rank = 1
    WHERE GameID = (
        SELECT GameID
        FROM display.shooting_main
        ORDER BY `Goal` DESC, `Accuracy` DESC
  LIMIT 1
    );
END
$$
DELIMITER ;
DROP TRIGGER IF EXISTS `update_shooting_recent`;
DELIMITER $$
CREATE TRIGGER `update_shooting_recent` AFTER INSERT ON `shooting` FOR EACH ROW BEGIN
    -- Delete the previous rows in shooting_recent
    DELETE FROM `display`.`shooting_recent`;

    -- Insert the latest row from shooting into shooting_recent
    INSERT INTO `display`.`shooting_recent` (`GameID`, `ID`, `PlayerName`, `OverallRank`, `Rank`, `Shoot`, `Goal`, `Miss`,`Accuracy`, `Age`, `EntryDate`)
    SELECT NEW.`GameID`, NEW.`ID`, NEW.`PlayerName`, NEW.`OverallRank`, NEW.`Rank`, NEW.`Shoot`, NEW.`Goal`, NEW.`Miss`, NEW.`Accuracy`, NEW.`Age`, NEW.`EntryDate`
    FROM `display`.`shooting`
    WHERE NEW.`ID` = `display`.`shooting`.`ID`
    ORDER BY NEW.`EntryDate` DESC
    LIMIT 1;

    -- Update the Rank and OverallRank columns in shooting_recent using data from table shooting
    UPDATE `display`.`shooting_recent` AS ar
    JOIN `display`.`shooting` AS a ON ar.GameID = a.GameID
    SET ar.`Rank` = a.`Rank`,
        ar.`OverallRank` = a.`OverallRank`;
END
$$
DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `shooting_main`
--

DROP TABLE IF EXISTS `shooting_main`;
CREATE TABLE `shooting_main` (
  `GameID` int(255) NOT NULL,
  `ID` varchar(55) NOT NULL,
  `OverallRank` int(11) NOT NULL,
  `Rank` int(11) NOT NULL,
  `PlayerName` varchar(255) NOT NULL,
  `Shoot` int(11) NOT NULL,
  `Goal` int(11) NOT NULL,
  `Miss` int(11) NOT NULL,
  `Accuracy` float NOT NULL,
  `Age` int(5) NOT NULL,
  `EntryDate` datetime NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `shooting_main`
--

INSERT INTO `shooting_main` (`GameID`, `ID`, `OverallRank`, `Rank`, `PlayerName`, `Shoot`, `Goal`, `Miss`, `Accuracy`, `Age`, `EntryDate`) VALUES
(38, 'C32D2C40', 2, 2, 'Sinta', 9, 7, 2, 77.78, 2, '2024-02-08 10:36:17'),
(39, '3A9A6D16', 4, 3, 'Muzakki', 1, 0, 1, 0, 23, '2024-02-08 11:02:55'),
(40, '10A83A0E', 1, 1, 'Steven', 11, 8, 3, 72.73, 0, '2024-02-08 11:03:38');

-- --------------------------------------------------------

--
-- Table structure for table `shooting_recent`
--

DROP TABLE IF EXISTS `shooting_recent`;
CREATE TABLE `shooting_recent` (
  `GameID` int(255) NOT NULL,
  `ID` varchar(55) NOT NULL,
  `OverallRank` int(11) NOT NULL,
  `Rank` int(11) NOT NULL,
  `PlayerName` varchar(255) NOT NULL,
  `Shoot` int(11) NOT NULL,
  `Goal` int(11) NOT NULL,
  `Miss` int(11) NOT NULL,
  `Accuracy` float NOT NULL,
  `Age` int(5) NOT NULL,
  `EntryDate` datetime NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `shooting_recent`
--

INSERT INTO `shooting_recent` (`GameID`, `ID`, `OverallRank`, `Rank`, `PlayerName`, `Shoot`, `Goal`, `Miss`, `Accuracy`, `Age`, `EntryDate`) VALUES
(40, '10A83A0E', 1, 1, 'Steven', 11, 8, 3, 72.73, 0, '2024-02-08 11:03:38');

-- --------------------------------------------------------

--
-- Table structure for table `smart_wall`
--

DROP TABLE IF EXISTS `smart_wall`;
CREATE TABLE `smart_wall` (
  `GameID` int(255) NOT NULL,
  `ID` varchar(55) NOT NULL,
  `OverallRank` int(11) NOT NULL,
  `Rank` int(11) NOT NULL,
  `PlayerName` varchar(255) NOT NULL,
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

INSERT INTO `smart_wall` (`GameID`, `ID`, `OverallRank`, `Rank`, `PlayerName`, `Point`, `Wrong`, `Score`, `Accuracy`, `Average`, `Fastest`, `Slowest`, `Age`, `EntryDate`) VALUES
(238, '6A59EDAF', 14, 2, 'Player G', 20, 0, 0, 76.92, 1.208, 0.713, 3.725, 2, '2024-05-16 10:10:45'),
(239, '97918B7A', 56, 5, 'Player V', 13, 0, 0, 86.67, 2.086, 0.486, 4.547, 9, '2024-05-16 10:11:56'),
(240, 'E76D9A7B', 5, 1, 'Player A', 23, 0, 0, 92, 1.198, 0.808, 1.849, 1, '2024-05-16 10:13:18'),
(241, 'FED96E85', 59, 6, 'Aldrifa Farizki', 12, 0, 0, 75, 1.745, 1.106, 4.357, 3, '2024-07-05 17:14:45'),
(242, '16D57D8D', 53, 4, 'Player O', 14, 0, 0, 87.5, 1.811, 1.054, 2.528, 5, '2024-07-05 17:18:24'),
(243, '2637247E', 43, 3, 'Player S', 15, 0, 0, 83.33, 1.668, 0.872, 4.616, 9, '2024-07-05 17:20:50');

--
-- Triggers `smart_wall`
--
DROP TRIGGER IF EXISTS `refresh_smart_wall_main`;
DELIMITER $$
CREATE TRIGGER `refresh_smart_wall_main` AFTER INSERT ON `smart_wall` FOR EACH ROW BEGIN
    -- Delete existing entries from the display table
    DELETE FROM `display`.`smart_wall_main`;

    -- Insert new entries into the display table based on the highest "Score" for each player name and oldest entry date
    INSERT INTO `display`.`smart_wall_main` (`GameID`, `ID`, `PlayerName`, `OverallRank`, `Point`, `Wrong`, `Score`, `Accuracy`, `Average`,`Fastest`,`Slowest`, `Age`, `EntryDate`)  
    SELECT a.`GameID`, a.`ID`, a.`PlayerName`, a.`OverallRank`, a.`Point`, a.`Wrong`, a.`Score`, a.`Accuracy`, a.`Average`, a.`Fastest`,a.`Slowest`, a.`Age`, a.`EntryDate`
    FROM (
        SELECT `PlayerName`, MAX(`Point`) AS maxPoint
        FROM `display`.`smart_wall`
        GROUP BY `PlayerName`
    ) AS maxPointSubquery
    JOIN `display`.`smart_wall` AS a
    ON a.`PlayerName` = maxPointSubquery.`PlayerName` AND a.`Point` = maxPointSubquery.`maxPoint`
    AND a.`EntryDate` = (
        SELECT MIN(`EntryDate`)
        FROM `display`.`smart_wall` b
        WHERE b.`PlayerName` = a.`PlayerName` AND b.`Point` = a.`Point`
    );

    SET @rank = 0;

    -- Update the rank within "reaction_main" based on the highest Point for each unique "PlayerName"
    UPDATE `display`.`smart_wall_main` a
    SET a.Rank = (SELECT COUNT(*) + 1
        FROM `display`.`smart_wall_main` b
        WHERE (b.Point > a.Point OR (b.Point = a.Point AND b.Fastest < a.Fastest))
    );

    -- Set Rank to 1 for the row with the highest Point and fastest reaction time
UPDATE display.smart_wall_main
SET Rank = 1
WHERE GameID = (
  SELECT GameID
  FROM display.smart_wall_main
  ORDER BY `Point` DESC, `Fastest` ASC
  LIMIT 1
);

END
$$
DELIMITER ;
DROP TRIGGER IF EXISTS `update_smart_wall_recent`;
DELIMITER $$
CREATE TRIGGER `update_smart_wall_recent` AFTER INSERT ON `smart_wall` FOR EACH ROW BEGIN
    -- Delete the previous row in reaction_recent
    DELETE FROM display.smart_wall_recent;

    -- Insert the latest row from reaction into reaction_recent

 INSERT INTO `display`.`smart_wall_recent` (`GameID`, `ID`, `PlayerName`,`OverallRank`, `Point`, `Wrong`, `Score`,`Accuracy`,`Average`,`Fastest`,`Slowest`, `Age`,`EntryDate`)  
SELECT NEW.`GameID`,  NEW.`ID`, NEW.`PlayerName`, NEW.`OverallRank`, NEW.`Point`, NEW.`Wrong`, NEW.`Score`, NEW.`Accuracy`, NEW.`Average`,  NEW.`Fastest`,  NEW.`Slowest`, NEW.`Age`, NEW.`EntryDate`

    FROM display.smart_wall
    WHERE NEW.`ID` = smart_wall.`ID`
    ORDER BY NEW.`EntryDate` DESC
    LIMIT 1;
    
        -- Update the Rank and OverallRank columns in reaction_recent
    UPDATE display.smart_wall_recent AS ar
    JOIN display.smart_wall AS a ON ar.GameID = a.GameID
    SET ar.`Rank` = a.`Rank`,
        ar.`OverallRank` = a.`OverallRank`;
END
$$
DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `smart_wall_main`
--

DROP TABLE IF EXISTS `smart_wall_main`;
CREATE TABLE `smart_wall_main` (
  `GameID` int(255) NOT NULL,
  `ID` varchar(55) NOT NULL,
  `OverallRank` int(11) NOT NULL,
  `Rank` int(11) NOT NULL,
  `PlayerName` varchar(255) NOT NULL,
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
-- Dumping data for table `smart_wall_main`
--

INSERT INTO `smart_wall_main` (`GameID`, `ID`, `OverallRank`, `Rank`, `PlayerName`, `Point`, `Wrong`, `Score`, `Accuracy`, `Average`, `Fastest`, `Slowest`, `Age`, `EntryDate`) VALUES
(238, '6A59EDAF', 14, 2, 'Player G', 20, 0, 0, 76.92, 1.208, 0.713, 3.725, 2, '2024-05-16 10:10:45'),
(239, '97918B7A', 56, 5, 'Player V', 13, 0, 0, 86.67, 2.086, 0.486, 4.547, 9, '2024-05-16 10:11:56'),
(240, 'E76D9A7B', 5, 1, 'Player A', 23, 0, 0, 92, 1.198, 0.808, 1.849, 1, '2024-05-16 10:13:18'),
(241, 'FED96E85', 59, 6, 'Aldrifa Farizki', 12, 0, 0, 75, 1.745, 1.106, 4.357, 3, '2024-07-05 17:14:45'),
(242, '16D57D8D', 53, 4, 'Player O', 14, 0, 0, 87.5, 1.811, 1.054, 2.528, 5, '2024-07-05 17:18:24'),
(243, '2637247E', 43, 3, 'Player S', 15, 0, 0, 83.33, 1.668, 0.872, 4.616, 9, '2024-07-05 17:20:50');

-- --------------------------------------------------------

--
-- Table structure for table `smart_wall_recent`
--

DROP TABLE IF EXISTS `smart_wall_recent`;
CREATE TABLE `smart_wall_recent` (
  `GameID` int(255) NOT NULL,
  `ID` varchar(55) NOT NULL,
  `OverallRank` int(11) NOT NULL,
  `Rank` int(11) NOT NULL,
  `PlayerName` varchar(255) NOT NULL,
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
-- Dumping data for table `smart_wall_recent`
--

INSERT INTO `smart_wall_recent` (`GameID`, `ID`, `OverallRank`, `Rank`, `PlayerName`, `Point`, `Wrong`, `Score`, `Accuracy`, `Average`, `Fastest`, `Slowest`, `Age`, `EntryDate`) VALUES
(243, '2637247E', 43, 3, 'Player S', 15, 0, 0, 83.33, 1.668, 0.872, 4.616, 9, '2024-07-05 17:20:50');

-- --------------------------------------------------------

--
-- Table structure for table `sprint_new`
--

DROP TABLE IF EXISTS `sprint_new`;
CREATE TABLE `sprint_new` (
  `GameID` int(255) NOT NULL,
  `ID` varchar(55) NOT NULL,
  `OverallRank` int(11) NOT NULL,
  `Rank` int(11) DEFAULT NULL,
  `PlayerName` varchar(255) NOT NULL,
  `Time` float NOT NULL,
  `AvgSpeed` float NOT NULL,
  `TopSpeed` float NOT NULL,
  `TopAcceleration` float NOT NULL,
  `Age` int(5) NOT NULL,
  `EntryDate` datetime NOT NULL,
  `Source` varchar(5) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `sprint_new`
--

INSERT INTO `sprint_new` (`GameID`, `ID`, `OverallRank`, `Rank`, `PlayerName`, `Time`, `AvgSpeed`, `TopSpeed`, `TopAcceleration`, `Age`, `EntryDate`, `Source`) VALUES
(411, '10A83A0E', 7, 7, 'Steven', 13.92, 5.18, 5.48, 0.02, 0, '2024-02-08 11:02:43', 'Right'),
(412, '22D9BCFC', 9, 9, 'Tiara', 18.83, 3.9, 4.73, 0.05, 15, '2024-02-08 12:05:19', 'Left'),
(413, '10A83A0E', 8, 8, 'Steven', 18.67, 4.09, 5.81, 0.09, 0, '2024-02-08 12:05:20', 'Right'),
(414, '10A83A0E', 2, 2, 'Steven', 2.86, 26.43, 31.3, 2.1, 0, '2024-02-08 12:06:46', 'Right'),
(415, '3A9A6D16', 4, 4, 'Muzakki', 3.54, 21.39, 25.17, 1.53, 23, '2024-02-08 12:06:47', 'Left'),
(416, '10A83A0E', 1, 1, 'Steven', 2.86, 26.32, 31.36, 2, 0, '2024-02-08 12:08:09', 'Right'),
(417, '3A9A6D16', 6, 6, 'Muzakki', 3.69, 19.68, 21.45, 0.67, 23, '2024-02-08 12:08:10', 'Left'),
(418, '3A9A6D16', 3, 3, 'Muzakki', 3.32, 22.64, 26.63, 1.64, 23, '2024-02-08 12:09:50', 'Left'),
(419, '10A83A0E', 5, 5, 'Steven', 3.58, 20.84, 24.83, 1.09, 0, '2024-02-08 12:09:50', 'Right');

--
-- Triggers `sprint_new`
--
DROP TRIGGER IF EXISTS `refresh_sprint_new_left`;
DELIMITER $$
CREATE TRIGGER `refresh_sprint_new_left` AFTER INSERT ON `sprint_new` FOR EACH ROW BEGIN
    -- Check if the inserted row has Source='Left'
    IF NEW.Source = 'Left' THEN
        -- Delete existing entries from the `sprint_new_left` table
        DELETE FROM `display`.`sprint_new_left`;

        -- Insert the latest row with Source='Left' into the `sprint_new_left` table
        INSERT INTO `display`.`sprint_new_left` (`GameID`, `ID`, `OverallRank`, `Rank`, `PlayerName`, `Time`, `AvgSpeed`,`TopSpeed`,`TopAcceleration`, `Age`, `EntryDate`, `Source`)
        SELECT s.`GameID`, s.`ID`, s.`OverallRank`, s.`Rank`, s.`PlayerName`, s.`Time`, s.`AvgSpeed`, s.`TopSpeed`, s.`TopAcceleration`, s.`Age`,s.`EntryDate`, s.`Source`
        FROM `display`.`sprint_new` AS s
        WHERE s.`Source` = 'Left'
        ORDER BY s.`EntryDate` DESC
        LIMIT 1;
    END IF;

        -- Update the Rank and OverallRank columns in agility_recent
    UPDATE display.sprint_new_left AS ar
    JOIN display.sprint_new AS a ON ar.GameID = a.GameID
    SET ar.`Rank` = a.`Rank`,
        ar.`OverallRank` = a.`OverallRank`;
END
$$
DELIMITER ;
DROP TRIGGER IF EXISTS `refresh_sprint_new_main`;
DELIMITER $$
CREATE TRIGGER `refresh_sprint_new_main` AFTER INSERT ON `sprint_new` FOR EACH ROW BEGIN
    -- Delete existing entries from the display table
    DELETE FROM `display`.`sprint_new_main`;

    -- Insert new entries into the display table based on the lowest "Time" for each player name and oldest entry date
    INSERT INTO `display`.`sprint_new_main` (`PlayerName`, `Time`, `ID`, `EntryDate`, `Age`, `GameID`, `OverallRank`,`AvgSpeed`,`TopSpeed`,`TopAcceleration`)
    SELECT
        a.`PlayerName`,
        a.`Time`,
        a.`ID`,
        a.`EntryDate`,
        a.`Age`,
        a.`GameID`,
        a.`OverallRank`,
        a.`AvgSpeed`,
        a.`TopSpeed`,
        a.`TopAcceleration`
    FROM (
        SELECT `PlayerName`, MIN(`Time`) AS minTime
        FROM `display`.`sprint_new`
        GROUP BY `PlayerName`
    ) AS minTimeSubquery
    JOIN `display`.`sprint_new` AS a
    ON a.`PlayerName` = minTimeSubquery.`PlayerName` AND a.`Time` = minTimeSubquery.`minTime`
    AND a.`EntryDate` = (
        SELECT MIN(`EntryDate`)
        FROM `display`.`sprint_new` b
        WHERE b.`PlayerName` = a.`PlayerName` AND b.`Time` = a.`Time`
    );

    SET @rank = 0;

    -- Update the rank within "sprint_new_main" based on the lowest "Time" for each unique "PlayerName"
-- Update the rank in "sprint_new_main" and consider only the row with the lowest "Time" for the same "ID" from "sprint_new"
-- Update Rank for the "sprint_new_main" table only
UPDATE display.sprint_new_main a
SET a.Rank = (
  SELECT COUNT(*) + 1
  FROM display.sprint_new_main b
  WHERE (b.Time < a.Time OR (b.Time = a.Time AND b.EntryDate < a.EntryDate))
);

-- Set Rank to 1 for the row with the lowest Time and oldest EntryDate
UPDATE display.sprint_new_main
SET Rank = 1
WHERE GameID = (
  SELECT GameID
  FROM display.sprint_new_main
  ORDER BY Time, EntryDate
  LIMIT 1
);

END
$$
DELIMITER ;
DROP TRIGGER IF EXISTS `refresh_sprint_new_right`;
DELIMITER $$
CREATE TRIGGER `refresh_sprint_new_right` AFTER INSERT ON `sprint_new` FOR EACH ROW BEGIN
    -- Check if the inserted row has Source='Right'
    IF NEW.Source = 'Right' THEN
        -- Delete existing entries from the `sprint_new_Right` table
        DELETE FROM `display`.`sprint_new_right`;

        -- Insert the latest row with Source='Right' into the `sprint_new_right` table
        INSERT INTO `display`.`sprint_new_right` (`GameID`, `ID`, `OverallRank`, `Rank`, `PlayerName`, `Time`, `AvgSpeed`,`TopSpeed`, `TopAcceleration`, `Age`, `EntryDate`, `Source`)
        SELECT s.`GameID`, s.`ID`, s.`OverallRank`, s.`Rank`, s.`PlayerName`, s.`Time`, s.`AvgSpeed`,  s.`TopSpeed`, s.`TopAcceleration`, s.`Age`, s.`EntryDate`, s.`Source`
        FROM `display`.`sprint_new` AS s
        WHERE s.`Source` = 'Right'
        ORDER BY s.`EntryDate` DESC
        LIMIT 1;
    END IF;

        -- Update the Rank and OverallRank columns in agility_recent
    UPDATE display.sprint_new_right AS ar
    JOIN display.sprint_new AS a ON ar.GameID = a.GameID
    SET ar.`Rank` = a.`Rank`,
        ar.`OverallRank` = a.`OverallRank`;
END
$$
DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `sprint_new_left`
--

DROP TABLE IF EXISTS `sprint_new_left`;
CREATE TABLE `sprint_new_left` (
  `GameID` int(255) NOT NULL,
  `ID` varchar(55) NOT NULL,
  `OverallRank` int(11) NOT NULL,
  `Rank` int(11) DEFAULT NULL,
  `PlayerName` varchar(255) NOT NULL,
  `Time` float NOT NULL,
  `AvgSpeed` float NOT NULL,
  `TopSpeed` float NOT NULL,
  `TopAcceleration` float NOT NULL,
  `Age` int(5) NOT NULL,
  `EntryDate` datetime NOT NULL,
  `Source` varchar(5) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `sprint_new_left`
--

INSERT INTO `sprint_new_left` (`GameID`, `ID`, `OverallRank`, `Rank`, `PlayerName`, `Time`, `AvgSpeed`, `TopSpeed`, `TopAcceleration`, `Age`, `EntryDate`, `Source`) VALUES
(418, '3A9A6D16', 3, 3, 'Muzakki', 3.32, 22.64, 26.63, 1.64, 23, '2024-02-08 12:09:50', 'Left');

-- --------------------------------------------------------

--
-- Table structure for table `sprint_new_main`
--

DROP TABLE IF EXISTS `sprint_new_main`;
CREATE TABLE `sprint_new_main` (
  `GameID` int(255) NOT NULL,
  `ID` varchar(55) NOT NULL,
  `OverallRank` int(11) NOT NULL,
  `Rank` int(11) DEFAULT NULL,
  `PlayerName` varchar(255) NOT NULL,
  `Time` float NOT NULL,
  `AvgSpeed` float NOT NULL,
  `TopSpeed` float NOT NULL,
  `TopAcceleration` float NOT NULL,
  `Age` int(5) NOT NULL,
  `EntryDate` datetime NOT NULL,
  `Source` varchar(5) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `sprint_new_main`
--

INSERT INTO `sprint_new_main` (`GameID`, `ID`, `OverallRank`, `Rank`, `PlayerName`, `Time`, `AvgSpeed`, `TopSpeed`, `TopAcceleration`, `Age`, `EntryDate`, `Source`) VALUES
(412, '22D9BCFC', 9, 3, 'Tiara', 18.83, 3.9, 4.73, 0.05, 15, '2024-02-08 12:05:19', ''),
(414, '10A83A0E', 2, 1, 'Steven', 2.86, 26.43, 31.3, 2.1, 0, '2024-02-08 12:06:46', ''),
(418, '3A9A6D16', 3, 2, 'Muzakki', 3.32, 22.64, 26.63, 1.64, 23, '2024-02-08 12:09:50', '');

-- --------------------------------------------------------

--
-- Table structure for table `sprint_new_right`
--

DROP TABLE IF EXISTS `sprint_new_right`;
CREATE TABLE `sprint_new_right` (
  `GameID` int(255) NOT NULL,
  `ID` varchar(55) NOT NULL,
  `OverallRank` int(11) NOT NULL,
  `Rank` int(11) DEFAULT NULL,
  `PlayerName` varchar(255) NOT NULL,
  `Time` float NOT NULL,
  `AvgSpeed` float NOT NULL,
  `TopSpeed` float NOT NULL,
  `TopAcceleration` float NOT NULL,
  `Age` int(5) NOT NULL,
  `EntryDate` datetime NOT NULL,
  `Source` varchar(5) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `sprint_new_right`
--

INSERT INTO `sprint_new_right` (`GameID`, `ID`, `OverallRank`, `Rank`, `PlayerName`, `Time`, `AvgSpeed`, `TopSpeed`, `TopAcceleration`, `Age`, `EntryDate`, `Source`) VALUES
(419, '10A83A0E', 5, 5, 'Steven', 3.58, 20.84, 24.83, 1.09, 0, '2024-02-08 12:09:50', 'Right');

--
-- Indexes for dumped tables
--

--
-- Indexes for table `agility`
--
ALTER TABLE `agility`
  ADD PRIMARY KEY (`GameID`),
  ADD KEY `ID` (`ID`);

--
-- Indexes for table `agility_main`
--
ALTER TABLE `agility_main`
  ADD PRIMARY KEY (`GameID`),
  ADD KEY `ID` (`ID`);

--
-- Indexes for table `agility_recent`
--
ALTER TABLE `agility_recent`
  ADD PRIMARY KEY (`GameID`),
  ADD KEY `ID` (`ID`);

--
-- Indexes for table `reaction`
--
ALTER TABLE `reaction`
  ADD PRIMARY KEY (`GameID`),
  ADD KEY `ID` (`ID`);

--
-- Indexes for table `reaction_main`
--
ALTER TABLE `reaction_main`
  ADD PRIMARY KEY (`GameID`),
  ADD KEY `ID` (`ID`);

--
-- Indexes for table `reaction_recent`
--
ALTER TABLE `reaction_recent`
  ADD PRIMARY KEY (`GameID`),
  ADD KEY `ID` (`ID`);

--
-- Indexes for table `shooting`
--
ALTER TABLE `shooting`
  ADD PRIMARY KEY (`GameID`),
  ADD KEY `ID` (`ID`);

--
-- Indexes for table `shooting_main`
--
ALTER TABLE `shooting_main`
  ADD PRIMARY KEY (`GameID`),
  ADD KEY `ID` (`ID`);

--
-- Indexes for table `shooting_recent`
--
ALTER TABLE `shooting_recent`
  ADD PRIMARY KEY (`GameID`),
  ADD KEY `ID` (`ID`);

--
-- Indexes for table `smart_wall`
--
ALTER TABLE `smart_wall`
  ADD PRIMARY KEY (`GameID`),
  ADD KEY `ID` (`ID`);

--
-- Indexes for table `smart_wall_main`
--
ALTER TABLE `smart_wall_main`
  ADD PRIMARY KEY (`GameID`),
  ADD KEY `ID` (`ID`);

--
-- Indexes for table `smart_wall_recent`
--
ALTER TABLE `smart_wall_recent`
  ADD PRIMARY KEY (`GameID`),
  ADD KEY `ID` (`ID`);

--
-- Indexes for table `sprint_new`
--
ALTER TABLE `sprint_new`
  ADD PRIMARY KEY (`GameID`),
  ADD KEY `ID` (`ID`);

--
-- Indexes for table `sprint_new_left`
--
ALTER TABLE `sprint_new_left`
  ADD PRIMARY KEY (`GameID`),
  ADD KEY `ID` (`ID`);

--
-- Indexes for table `sprint_new_main`
--
ALTER TABLE `sprint_new_main`
  ADD PRIMARY KEY (`GameID`),
  ADD KEY `ID` (`ID`);

--
-- Indexes for table `sprint_new_right`
--
ALTER TABLE `sprint_new_right`
  ADD PRIMARY KEY (`GameID`),
  ADD KEY `ID` (`ID`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `agility`
--
ALTER TABLE `agility`
  MODIFY `GameID` int(255) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=134;

--
-- AUTO_INCREMENT for table `agility_main`
--
ALTER TABLE `agility_main`
  MODIFY `GameID` int(255) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=133;

--
-- AUTO_INCREMENT for table `agility_recent`
--
ALTER TABLE `agility_recent`
  MODIFY `GameID` int(255) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=134;

--
-- AUTO_INCREMENT for table `reaction`
--
ALTER TABLE `reaction`
  MODIFY `GameID` int(255) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=318;

--
-- AUTO_INCREMENT for table `reaction_main`
--
ALTER TABLE `reaction_main`
  MODIFY `GameID` int(255) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=318;

--
-- AUTO_INCREMENT for table `reaction_recent`
--
ALTER TABLE `reaction_recent`
  MODIFY `GameID` int(255) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=318;

--
-- AUTO_INCREMENT for table `shooting`
--
ALTER TABLE `shooting`
  MODIFY `GameID` int(255) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=41;

--
-- AUTO_INCREMENT for table `shooting_main`
--
ALTER TABLE `shooting_main`
  MODIFY `GameID` int(255) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=41;

--
-- AUTO_INCREMENT for table `shooting_recent`
--
ALTER TABLE `shooting_recent`
  MODIFY `GameID` int(255) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=41;

--
-- AUTO_INCREMENT for table `smart_wall`
--
ALTER TABLE `smart_wall`
  MODIFY `GameID` int(255) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=244;

--
-- AUTO_INCREMENT for table `smart_wall_main`
--
ALTER TABLE `smart_wall_main`
  MODIFY `GameID` int(255) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=244;

--
-- AUTO_INCREMENT for table `smart_wall_recent`
--
ALTER TABLE `smart_wall_recent`
  MODIFY `GameID` int(255) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=244;

DELIMITER $$
--
-- Events
--
DROP EVENT IF EXISTS `rank_agility`$$
CREATE DEFINER=`root`@`localhost` EVENT `rank_agility` ON SCHEDULE EVERY 1 SECOND STARTS '2023-08-16 16:03:58' ON COMPLETION PRESERVE ENABLE DO BEGIN
    -- Update Rank in agility
    UPDATE `display`.`agility` AS s
    JOIN (
        SELECT
            `EntryDate`,
            `FinalTime`,
            ROW_NUMBER() OVER (ORDER BY `FinalTime`, `EntryDate` DESC) AS Rank
        FROM
            `display`.`agility`
    ) AS t ON s.`EntryDate` = t.`EntryDate` AND s.`FinalTime` = t.`FinalTime`
    SET s.`Rank` = t.`Rank`;

    -- Update OverallRank in agility_main from agility
    UPDATE display.agility_main AS a
    JOIN display.agility AS u ON a.GameID = u.GameID
    SET a.`OverallRank` = u.`OverallRank`;

-- Update Rank for the "agility_main" table only
UPDATE display.agility_main a
SET a.Rank = (
  SELECT COUNT(*) + 1
  FROM display.agility_main b
  WHERE (b.FinalTime < a.FinalTime OR (b.FinalTime = a.FinalTime AND b.EntryDate < a.EntryDate))
);

-- Set Rank to 1 for the row with the lowest FinalTime and oldest EntryDate
UPDATE display.agility_main
SET Rank = 1
WHERE GameID = (
  SELECT GameID
  FROM display.agility_main
  ORDER BY FinalTime, EntryDate
  LIMIT 1
);


        -- Update the Rank and OverallRank columns in agility_recent
    UPDATE display.agility_recent AS ar
    JOIN display.agility AS a ON ar.GameID = a.GameID
    SET ar.`Rank` = a.`Rank`,
        ar.`OverallRank` = a.`OverallRank`;
END$$

DROP EVENT IF EXISTS `rank_reaction`$$
CREATE DEFINER=`root`@`localhost` EVENT `rank_reaction` ON SCHEDULE EVERY 1 SECOND STARTS '2023-08-16 16:03:58' ON COMPLETION PRESERVE ENABLE DO BEGIN
    -- Update Rank in reaction
    UPDATE `display`.`reaction` AS s
    JOIN (
        SELECT
            `Fastest`,
            `Point`,
            ROW_NUMBER() OVER (ORDER BY `Point` DESC, `Fastest` ASC) AS Rank
        FROM
            `display`.`reaction`
    ) AS t ON s.`Fastest` = t.`Fastest` AND s.`Point` = t.`Point`
    SET s.`Rank` = t.`Rank`;

    -- Update OverallRank in reaction_main from reaction
    UPDATE display.reaction_main AS a
    JOIN display.reaction AS u ON a.GameID = u.GameID
    SET a.`OverallRank` = u.`OverallRank`;

-- Update Rank for the "reaction_main" table only
UPDATE display.reaction_main a
SET a.Rank = (
  SELECT COUNT(*) + 1
  FROM display.reaction_main b
  WHERE (b.Point > a.Point OR (b.Point = a.Point AND b.Fastest < a.Fastest))
);

-- Set Rank to 1 for the row with the highest Point and oldest Fastest
UPDATE display.reaction_main
SET Rank = 1
WHERE GameID = (
  SELECT GameID
  FROM display.reaction_main
  ORDER BY `Point` DESC, `Fastest` ASC
  LIMIT 1
);


        -- Update the Rank and OverallRank columns in reaction_recent using data from reaction table
    UPDATE display.reaction_recent AS ar
    JOIN display.reaction AS a ON ar.GameID = a.GameID
    SET ar.`Rank` = a.`Rank`,
        ar.`OverallRank` = a.`OverallRank`;
END$$

DROP EVENT IF EXISTS `rank_shooting`$$
CREATE DEFINER=`root`@`localhost` EVENT `rank_shooting` ON SCHEDULE EVERY 1 SECOND STARTS '2023-08-16 16:03:58' ON COMPLETION PRESERVE ENABLE DO BEGIN
    -- Update Rank in shooting
    UPDATE `display`.`shooting` AS s
    JOIN (
        SELECT
            `Accuracy`,
            `Goal`,
            ROW_NUMBER() OVER (ORDER BY `Goal` DESC, `Accuracy` DESC) AS Rank
        FROM
            `display`.`shooting`
    ) AS t ON s.`Accuracy` = t.`Accuracy` AND s.`Goal` = t.`Goal`
    SET s.`Rank` = t.`Rank`;

    -- Update OverallRank in shooting_main from shooting
    UPDATE display.shooting_main AS a
    JOIN display.shooting AS u ON a.GameID = u.GameID
    SET a.`OverallRank` = u.`OverallRank`;

    -- Update Rank for the "shooting_main" table only
    UPDATE display.shooting_main a
    SET a.Rank = (
      SELECT COUNT(*) + 1
      FROM display.shooting_main b
      WHERE (b.Goal > a.Goal OR (b.Goal = a.Goal AND b.Accuracy > a.Accuracy))
    );

    -- Set Rank to 1 for the row with the highest Score and oldest Accuracy
    UPDATE display.shooting_main
    SET Rank = 1
    WHERE GameID = (
      SELECT GameID
      FROM display.shooting_main
      ORDER BY `Goal` DESC, `Accuracy` DESC
      LIMIT 1
    );

    -- Update the Rank and OverallRank columns in shooting_recent using data from table shooting
    UPDATE display.shooting_recent AS ar
    JOIN display.shooting AS a ON ar.GameID = a.GameID
    SET ar.`Rank` = a.`Rank`,
        ar.`OverallRank` = a.`OverallRank`;
END$$

DROP EVENT IF EXISTS `rank_smart_wall`$$
CREATE DEFINER=`root`@`localhost` EVENT `rank_smart_wall` ON SCHEDULE EVERY 1 SECOND STARTS '2023-08-16 16:03:58' ON COMPLETION PRESERVE ENABLE DO BEGIN
    -- Update Rank in smart_wall
    UPDATE `display`.`smart_wall` AS s
    JOIN (
        SELECT
            `Fastest`,
            `Point`,
            ROW_NUMBER() OVER (ORDER BY `Point` DESC, `Fastest` ASC) AS Rank
        FROM
            `display`.`smart_wall`
    ) AS t ON s.`Fastest` = t.`Fastest` AND s.`Point` = t.`Point`
    SET s.`Rank` = t.`Rank`;

    -- Update OverallRank in smart_wall_main from smart_wall
    UPDATE display.smart_wall_main AS a
    JOIN display.smart_wall AS u ON a.GameID = u.GameID
    SET a.`OverallRank` = u.`OverallRank`;

-- Update Rank for the "smart_wall_main" table only
UPDATE display.smart_wall_main a
SET a.Rank = (
  SELECT COUNT(*) + 1
  FROM display.smart_wall_main b
  WHERE (b.Point > a.Point OR (b.Point = a.Point AND b.Fastest < a.Fastest))
);

-- Set Rank to 1 for the row with the highest Point and oldest Fastest
UPDATE display.smart_wall_main
SET Rank = 1
WHERE GameID = (
  SELECT GameID
  FROM display.smart_wall_main
  ORDER BY `Point` DESC, `Fastest` ASC
  LIMIT 1
);


        -- Update the Rank and OverallRank columns in smart_wall_recent using data from smart_wall table
    UPDATE display.smart_wall_recent AS ar
    JOIN display.smart_wall AS a ON ar.GameID = a.GameID
    SET ar.`Rank` = a.`Rank`,
        ar.`OverallRank` = a.`OverallRank`;
END$$

DROP EVENT IF EXISTS `rank_sprint_new`$$
CREATE DEFINER=`root`@`localhost` EVENT `rank_sprint_new` ON SCHEDULE EVERY 1 SECOND STARTS '2023-08-11 12:01:28' ON COMPLETION PRESERVE ENABLE DO BEGIN
    -- Update Rank in sprint_new

    UPDATE `display`.`sprint_new` AS s
    JOIN (
        SELECT
            `EntryDate`,
            `Time`,
            ROW_NUMBER() OVER (ORDER BY `Time`, `EntryDate` DESC) AS Rank
        FROM
            `display`.`sprint_new`
    ) AS t ON s.`EntryDate` = t.`EntryDate` AND s.`Time` = t.`Time`
    SET s.`Rank` = t.`Rank`;

    -- Update OverallRank in sprint_new_main from sprint_new
    UPDATE display.sprint_new_main AS a
    JOIN display.sprint_new AS u ON a.GameID = u.GameID
    SET a.`OverallRank` = u.`OverallRank`;

-- Update Rank for the "sprint_new_main" table only
UPDATE display.sprint_new_main a
SET a.Rank = (
  SELECT COUNT(*) + 1
  FROM display.sprint_new_main b
  WHERE (b.Time < a.Time OR (b.Time = a.Time AND b.EntryDate < a.EntryDate))
);

-- Set Rank to 1 for the row with the lowest Time and oldest EntryDate
UPDATE display.sprint_new_main
SET Rank = 1
WHERE GameID = (
  SELECT GameID
  FROM display.sprint_new_main
  ORDER BY Time, EntryDate
  LIMIT 1
);

        -- Update the Rank and OverallRank columns in sprint_new_left
    UPDATE display.sprint_new_left AS ar
    JOIN display.sprint_new AS a ON ar.GameID = a.GameID
    SET ar.`Rank` = a.`Rank`,
        ar.`OverallRank` = a.`OverallRank`;
        
                -- Update the Rank and OverallRank columns in sprint_new_right
    UPDATE display.sprint_new_right AS ar
    JOIN display.sprint_new AS a ON ar.GameID = a.GameID
    SET ar.`Rank` = a.`Rank`,
        ar.`OverallRank` = a.`OverallRank`;
END$$

DELIMITER ;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
