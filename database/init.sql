CREATE DATABASE IF NOT EXISTS `qt_project`;
USE `qt_project`;

CREATE TABLE user_info(
  id int primary key auto_increment,
  name varchar(32) not null,
  pwd varchar(32) not null,
  online tinyint(1) default 0
);

CREATE TABLE friend(
  id int primary key auto_increment,
  user_id int not null,
  friend_id int not null,
  foreign key(user_id) references user_info(id),
  foreign key(friend_id) references user_info(id)
);
