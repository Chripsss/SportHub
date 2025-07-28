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
-- Database: `registration`
--
CREATE DATABASE IF NOT EXISTS `registration` DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;
USE `registration`;

-- --------------------------------------------------------

--
-- Table structure for table `user`
--

DROP TABLE IF EXISTS `user`;
CREATE TABLE `user` (
  `ID` int(11) NOT NULL,
  `UID` varchar(255) NOT NULL,
  `PlayerName` varchar(255) NOT NULL,
  `DoB` date NOT NULL,
  `Email` varchar(255) NOT NULL,
  `WhatsApp` int(20) NOT NULL,
  `MembershipType` varchar(50) NOT NULL,
  `EntryDate` datetime DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `user`
--

INSERT INTO `user` (`ID`, `UID`, `PlayerName`, `DoB`, `Email`, `WhatsApp`, `MembershipType`, `EntryDate`) VALUES
(231207001, 'B69C9D2B', 'FAS Ilham', '2000-12-12', '-', 0, 'Daily', '2023-12-07 08:43:08'),
(231207002, '729DBEFC', 'Player G', '2020-12-12', '-', 0, 'Daily', '2023-12-07 08:44:17'),
(231207003, '3A22D616', 'Group 5', '2000-12-08', '-', 0, 'Daily', '2023-12-07 08:44:42'),
(231207004, '3A9A6D16', 'Group 1', '2000-10-10', '-', 0, 'Daily', '2023-12-07 08:45:10'),
(231207005, 'D09A9A32', 'Group 6', '2000-12-14', '-', 0, 'Daily', '2023-12-07 08:45:39'),
(231207006, '06036F90', 'Player K', '2000-01-01', '-', 0, 'Daily', '2023-12-07 08:47:25'),
(231207007, '1CF2C738', 'Player L', '2000-01-02', '-', 0, 'Daily', '2023-12-07 08:48:29'),
(231207008, '81EE551D', 'Player M', '2000-11-03', '-', 0, 'Daily', '2023-12-07 08:48:59'),
(231207009, 'A191321D', 'Player N', '2000-01-01', '-', 0, 'Daily', '2023-12-07 08:49:22'),
(231207010, '16D57D8D', 'Player O', '2000-12-14', '-', 0, 'Daily', '2023-12-07 08:49:51'),
(231207011, 'DA73FDAF', 'Player P', '2000-01-04', '-', 0, 'Daily', '2023-12-07 08:50:53'),
(231207012, '4CD30839', 'Player Q', '2000-05-05', '-', 0, 'Daily', '2023-12-07 08:51:32'),
(231207013, '0C131239', 'Player R', '2003-01-01', '-', 0, 'Daily', '2023-12-07 10:32:27'),
(231207014, '2637247E', 'Player S', '2004-03-01', '-', 0, 'Daily', '2023-12-07 10:33:09'),
(231207015, 'FAE10CB0', 'Player T', '2006-01-04', '-', 0, 'Daily', '2023-12-07 10:34:09'),
(231207016, '91EEFA1D', 'Player U ', '2001-03-02', '-', 0, 'Daily', '2023-12-07 10:53:12'),
(231207017, '97918B7A', 'Player V', '2014-11-01', '-', 0, 'Daily', '2023-12-07 10:54:17'),
(231207018, '5AE10FB1', 'Player W', '2021-01-01', '-', 0, 'Daily', '2023-12-07 10:54:44'),
(231207019, 'BA1C26B0', 'Player X', '2001-11-14', '-', 0, 'Daily', '2023-12-07 10:57:01'),
(231207020, '76F27E8D', 'Player Y', '2001-04-05', '-', 0, 'Daily', '2023-12-07 10:57:22'),
(231207021, 'A15D2F1D', 'Player Z', '2021-01-04', '-', 0, 'Daily', '2023-12-07 10:58:01'),
(231207022, 'E76D9A7B', 'Player A', '2022-02-01', '-', 0, 'Daily', '2023-12-07 11:18:25'),
(231207023, '47C6AF7A', 'Player B', '2001-01-01', '-', 0, 'Daily', '2023-12-07 11:22:14'),
(231207024, '22D9BCFC', 'Group 2', '2008-04-01', '-', 0, 'Daily', '2023-12-07 11:24:07'),
(231207025, '2AE43D16', 'Group 4', '2003-02-03', '-', 0, 'Daily', '2023-12-07 11:25:17'),
(231208026, '5CCD8D34', 'Employee D', '2001-02-03', '-', 0, 'Employee', '2023-12-08 09:38:48'),
(231208027, 'DC0ED234', 'Annisa', '2001-04-04', '-', 0, 'Employee', '2023-12-08 09:47:57'),
(231208028, '2C649534', 'Janice', '2003-08-07', '-', 0, 'Employee', '2023-12-08 09:49:25'),
(231208029, '5A200B40', 'Employee B', '2001-01-01', '-', 0, 'Daily', '2023-12-08 09:51:12'),
(231215031, '10A83A0E', 'Nathan Wijaya', '2023-01-01', '-', 0, 'Daily', '2023-12-15 09:09:48'),
(231215032, '3AB1F616', 'Group 3', '2021-01-05', '-', 0, 'Daily', '2023-12-15 09:15:36'),
(231215033, '6A59EDAF', 'Player G', '2021-02-05', '-', 0, 'Daily', '2023-12-15 09:16:44'),
(231215034, 'D6776990', 'Player H', '2021-08-04', '-', 0, 'Daily', '2023-12-15 09:17:48'),
(231219035, '9EAF0B40', 'Aldrifa', '2021-01-01', '-', 0, 'Daily', '2023-12-19 10:51:34'),
(231219036, '19BD0B40', 'Employee C', '2001-09-08', '-', 0, 'Daily', '2023-12-19 10:56:22'),
(231219037, 'B31F0B40', 'Employee E', '2001-06-07', '-', 0, 'Daily', '2023-12-19 10:57:58'),
(231221038, 'C32D2C40', 'Sinta Widianingrum', '2000-12-21', '-', 0, 'Daily', '2023-12-21 08:49:35'),
(231221039, 'B2C70B40', 'ME Djati', '2012-01-04', '-', 0, 'Employee', '2023-12-21 10:21:55'),
(231221040, 'D358F965', 'Group 7', '2000-04-05', '-', 0, 'Employee', '2023-12-21 10:44:51'),
(231221041, 'FED96E85', 'Aldrifa Farizki', '2000-04-05', '-', 0, 'Employee', '2023-12-21 10:53:05'),
(231221042, '2AF22816', 'Group 8', '2000-04-05', '-', 0, 'Employee', '2023-12-21 10:55:22'),
(231221043, '4A3B4016', 'FET Tombak', '2021-01-01', '-', 0, 'Daily', '2023-12-21 11:06:37'),
(231221044, '4C729B34', 'Wahyu', '2000-01-01', '-', 0, 'Employee', '2023-12-21 11:55:52'),
(240514045, '56FD7F8D', 'Player C', '2001-01-01', '-', 0, 'Daily', '2024-05-14 15:09:26');

--
-- Triggers `user`
--
DROP TRIGGER IF EXISTS `after_delete_registration_user`;
DELIMITER $$
CREATE TRIGGER `after_delete_registration_user` AFTER DELETE ON `user` FOR EACH ROW BEGIN
    -- Delete the corresponding record in sportscience.user
    DELETE FROM sportscience.user WHERE ID = OLD.UID;
END
$$
DELIMITER ;
DROP TRIGGER IF EXISTS `after_insert_update_registration_user`;
DELIMITER $$
CREATE TRIGGER `after_insert_update_registration_user` AFTER INSERT ON `user` FOR EACH ROW BEGIN
    -- Check if the corresponding record exists in sportscience.user
    SET @user_exists = (SELECT COUNT(*) FROM sportscience.user WHERE ID = NEW.UID);

    -- If the user exists, update the record
    IF @user_exists > 0 THEN
        UPDATE sportscience.user
        SET
            PlayerName = NEW.PlayerName,
            DoB = NEW.DoB,
            Email = NEW.Email,
            WhatsApp = NEW.WhatsApp
        WHERE ID = NEW.UID;
    
    -- If the user doesn't exist, insert a new record
    ELSE
        INSERT INTO sportscience.user (ID, PlayerName, DoB, Email, WhatsApp)
        VALUES (NEW.UID, NEW.PlayerName, NEW.DoB, NEW.Email, NEW.WhatsApp);
    END IF;
END
$$
DELIMITER ;
DROP TRIGGER IF EXISTS `after_update_delete_empty_uid`;
DELIMITER $$
CREATE TRIGGER `after_update_delete_empty_uid` AFTER UPDATE ON `user` FOR EACH ROW BEGIN
    -- Check if the UID has changed and becomes "-" or empty
    IF OLD.UID != NEW.UID AND (NEW.UID IS NULL OR NEW.UID = "" OR NEW.UID = "-") THEN
        -- Delete all corresponding records in sportscience.user
        DELETE FROM sportscience.user WHERE ID = NEW.UID;
    END IF;
END
$$
DELIMITER ;
DROP TRIGGER IF EXISTS `after_update_registration_user`;
DELIMITER $$
CREATE TRIGGER `after_update_registration_user` AFTER UPDATE ON `user` FOR EACH ROW BEGIN
    -- Check if the UID has changed
    IF OLD.UID != NEW.UID THEN
        -- Delete the old corresponding record in sportscience.user
        DELETE FROM sportscience.user WHERE ID = OLD.UID;
        
        -- Check if the new UID already exists in registration.user
        SET @new_uid_exists = (SELECT COUNT(*) FROM registration.user WHERE ID = NEW.UID);

        -- If the new UID exists, update the record
        IF @new_uid_exists > 0 THEN
            UPDATE sportscience.user
            SET
                PlayerName = NEW.PlayerName,
                DoB = NEW.DoB,
                Email = NEW.Email,
                WhatsApp = NEW.WhatsApp
            WHERE ID = NEW.UID;
        
        -- If the new UID doesn't exist, insert a new record
        ELSE
            INSERT INTO sportscience.user (ID, PlayerName, DoB, Email, WhatsApp)
            VALUES (NEW.UID, NEW.PlayerName, NEW.DoB, NEW.Email, NEW.WhatsApp);
        END IF;
    -- If the UID has not changed, simply update the record
    ELSE
        UPDATE sportscience.user
        SET
            PlayerName = NEW.PlayerName,
            DoB = NEW.DoB,
            Email = NEW.Email,
            WhatsApp = NEW.WhatsApp
        WHERE ID = NEW.UID;
    END IF;
END
$$
DELIMITER ;

--
-- Indexes for dumped tables
--

--
-- Indexes for table `user`
--
ALTER TABLE `user`
  ADD PRIMARY KEY (`ID`);
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
