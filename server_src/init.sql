CREATE TABLE user_info (
    ID int(15) NOT NULL AUTO_INCREMENT,
    ServerName varchar(50) NOT NULL,
    ServerKey varchar(25) NOT NULL,
    PRIMARY KEY (`ID`)
) ENGINE=InnoDB;

CREATE TABLE IF NOT EXISTS `traffic_info` (
  `ID` int(15) NOT NULL AUTO_INCREMENT,
  `UserID` int(10) NOT NULL,
  `iface` varchar(25) NOT NULL,
  `year` int(5) NOT NULL,
  `month` int(3) NOT NULL,
  `day` int(3) NOT NULL,
  `in_dev` int(10) NOT NULL DEFAULT '0',
  `out_dev` int(10) NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`),
  UNIQUE KEY `UserID` (`UserID`,`iface`,`year`,`month`,`day`),
  KEY `iface` (`iface`),
  KEY `UserID_2` (`UserID`)
) ENGINE=InnoDB;

ALTER TABLE `traffic_info`
  ADD CONSTRAINT `traffic_info_uid` FOREIGN KEY (`UserID`) REFERENCES `user_info` (`ID`);