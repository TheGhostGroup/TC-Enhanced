SET @MAX_SEC := (SELECT MAX(security) FROM `command`);
DELETE FROM `command` WHERE name IN ('jail', 'jailinfo', 'unjail', 'pinfo', 'clearjail');
INSERT INTO `command` (name, security, help) VALUES
('jail',       1, 'Syntax: .jail $char_name $hours $reason\nJailed the \'character\' for \'hours\' with the \'reason\'.'),
('jailinfo',   0, 'Syntax: .jailinfo\nShows your jail status.'),
('unjail',     1, 'Syntax: .unjail $char_name\nRealeases the \'character\' out of the jail.'),
('clearjail',  @MAX_SEC, 'Syntax: .clearjail $char_name\nRemoves all jail records for character with entered name from DB.'),
('pinfo',      2, 'Syntax: .pinfo [$player_name] [rep] [jail]\nOutput account information for selected player or player find by $player_name.\nIf \'rep\' parameter is provided it shows the reputation information for players.\nIf \'jail\' parameter is provided it shows the jail information for players.');

DELETE FROM trinity_string WHERE `entry` BETWEEN '950' AND '975';
INSERT INTO trinity_string
   (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`)
VALUES
   (950, 'You are jailed by %s for %u hour(s)!', NULL, 'Vous tes emprisonn par %s pour %u heures!', 'Du wurdest von \'%s\' f?r %u Stunde(n) eingebuchtet!', NULL, NULL, NULL, NULL, 'Вы были посажены в тюрьму ГМом %s на %u час(а)!'),
   (951, '%s wrote this as reason: %s', NULL, '%s a crit ceci comme tant la raison de votre emprisonnement: %s.', '\'%s\' gab dies als Grund an: %s', NULL, NULL, NULL, NULL, '%s указал %s в качестве причины.'),
   (952, '%s was jailed by you for %u hour(s).', NULL, 'Vous avez emprisonn %s pour %u heures!', '\'%s\' wurde von dir f?r %u Stunde(n) eingebuchtet.', NULL, NULL, NULL, NULL, 'Вы посадили %s в тюрьму на %u час(ов).'),
   (953, 'You was released out of the jail by %s.', NULL, 'Vous avez t liber de la prison par %s.', '\'%s\' hat dich aus dem Knast entlassen.', NULL, NULL, NULL, NULL, 'Вы были освобождены из тюрьмы ГМом %s.'),
   (954, 'You have released %s out of the jail.', NULL, 'Vous avez liber %s de la prison.', 'Du hast \'%s\' aus dem Knast geholt.', NULL, NULL, NULL, NULL, 'Вы освободили %s из тюрьмы.'),
   (955, 'No reason given or reason is < %u chars!', NULL, 'Aucune raison d\\\'emprisonnement donne ou la raison est < %u personnages.', 'Du hast keinen Grund angegeben, oder der Grund ist < %u Zeichen!', NULL, NULL, NULL, NULL, 'Причина не была указана, или она оказалось меньше %u знаков!'),
   (956, 'No name given!', NULL, 'Aucun nom donn!', 'Du hast keinen Namen angegeben!', NULL, NULL, NULL, NULL, 'Не введено имя!'),
   (957, 'No time given!', NULL, 'Aucun temps donn!', 'Du hast keine Zeit angegeben!', NULL, NULL, NULL, NULL, 'Продолжительность заключения не указано!'),
   (958, 'The jailtime must be between 1 and %u hours!', NULL, 'Le temps d\\\'emprisonnement est situ entre 1 et %u heures!', 'Die Jail-Zeit muss zwischen 1 und %u Std. liegen!', NULL, NULL, NULL, NULL, 'Продолжительность заключения должна быть в пределах от 1 до %u часов!'),
   (959, 'The character %s is not jailed!', NULL, '%s n\\\'est pas emprisonn!', 'Der Charakter \'%s\' ist ?berhaupt nicht im Knast!', NULL, NULL, NULL, NULL, 'Персонаж %s не осуждён.'),
   (960, 'Command forbidden for jailed characters!', NULL, 'Commandes interdites pour les personnages emprisons!', 'Sorry, aber das d?rfen Gefangene nicht!', NULL, NULL, NULL, NULL, 'Команда недоступна для игроков, находящихся в тюрьме!'),
   (961, 'You have %u hour(s) left in the jail.', NULL, 'Vous avez %u heures  attendre avant de quitter la prison.', 'Du musst noch %u Stunde(n) absitzen.', NULL, NULL, NULL, NULL, 'До освобождения осталось %u часа(ов).'),
   (962, 'You have %u minute(s) left in the jail.', NULL, 'Vous avez %u minutes attendre avant de quitter la prison.', 'Du musst noch %u Minute(n) absitzen.', NULL, NULL, NULL, NULL, 'До освобождения осталось %u минут.'),
   (963, 'You\'re a free like a bird! ;-)', NULL, 'Vous tes libre.', 'Du bist frei wie ein Vogel! ;-)', NULL, NULL, NULL, NULL, 'Вы свободны... пока свободны.'),
   (964, '%s was %u times jailed and has %u minute(s) left. Last time jailed by %s. Last reason was: \'%s\'', NULL, '%s a t librde prison, il avait t emprisonn pour %u et a t libr au aprs %u minutes. Il avait t emprisonn par %s, pour la raison suivante: %s', '\'%s\' war bis jetzt %u mal im Knast, und hat noch %u Minute(n) abzusitzen.\n Zuletzt eingebuchtet von: \'%s\'\nLetzter Grund: %s', NULL, NULL, NULL, NULL, '%s был %u раз в тюрьме и ему осталось %u минут до освобождения. Последний раз осуждён %s. Последняя причина осуждения: \'%s\''),
   (965, '%s was never jailed.', NULL, '%s n\\\'a jamais t emprisonn.', '\'%s\' hat eine wei?e Weste.', NULL, NULL, NULL, NULL, '%s никогда не был в тюрьме.'),
   (966, 'You can\'t jail yourself!', NULL, 'Vous ne pouvez pas vous emprisonner vous-m me!', 'Du kannst dich nicht selber einbuchten!', NULL, NULL, NULL, NULL, 'Вы не можете посадить в тюрьму самого себя!'),
   (967, 'You can\'t unjail yourself!', NULL, 'Vous ne pouvez pas vous librer vous m me!', 'So weit kommt es noch, da? Knastbr?der sich selber befreien! :-(', NULL, NULL, NULL, NULL, 'Вы не можете освободить самого себя из тюрьмы!'),
   (968, '|cffff0000[!!! ATTENTION - IMPORTANT - WARNING !!!\r\n You were already %u times in prison beim %u mal your character will be deleted\r\n|cffff0000!!! ATTENTION - IMPORTANT - WARNING !!!]', NULL, '|cffff0000[!!!ATTENTION - ATTENTION - ATTENTION!!!\r\n Vous étiez déjà %u fois en prison en %u fois, votre personnage supprimé\r\n|cffff0000!!! ATTENTION - ATTENTION - ATTENTION !!!]', '|cffff0000[!!! ACHTUNG - WICHTIG - WARNUNG !!!\r\n Du warst schon %u mal in Knast beim %u mal wird dein Charakter gelöscht\r\n|cffff0000!!! ACHTUNG - WICHTIG - WARNUNG !!!]', NULL, NULL, NULL, NULL, '|cffff0000[!!! ВНИМАНИЕ - ВАЖНО - ВНИМАНИЕ !!!\r\n Ваш персонаж уже %u раз был в тюрьме. Через %u раз(а) он будет удалён!\r\n|cffff0000!!! ВНИМАНИЕ - ВАЖНО - ВНИМАНИЕ !!!]'),
   (969, 'The character \'%s\' was jailed for %s hour(s) by the GM character \'%s\'. The reason is: %s', NULL, 'Le personnage ', 'Der Charakter \'', NULL, NULL, NULL, NULL, 'Персонаж \'%s\' арестован на %s часа(ов) ГМом \'%s\'. Причина: %s'),
   (970, 'The character \'%s\' is jailed and teleported into the jail.', NULL, 'Le personnage %s  est emprisonn et t leport dans la prison.', 'Der Charakter \'%s\'  ist ein Knastbruder und wird in den Knast teleportiert.', NULL, NULL, NULL, NULL, 'Персонаж \'%s\' заключён и телепортирован в тюрьму.'),
   (971, 'The character \'%s\' was released out of the jail.', NULL, 'Le personnage %s  est liber  de prison.', 'Der Charakter \'%s\'  wurde aus dem Knast entlassen.', NULL, NULL, NULL, NULL, 'Персонаж \'%s\' освобождён из тюрьмы.'),
   (972, '|cffff0000[!!! ATTENTION - IMPORTANT - WARNING !!!\r\n You were already %u times in prison beim %u mal your account  will be banned!\r\n|cffff0000!!! ATTENTION - IMPORTANT - WARNING !!!]', NULL, '|cffff0000[!!!ATTENTION - ATTENTION - ATTENTION!!!\r\n Vous avez %u fois en prison en %u fois votre compte sera banni\r\n|cffff0000!!! ATTENTION - ATTENTION - ATTENTION !!!]', '|cffff0000[!!! ACHTUNG - WICHTIG - WARNUNG !!!\r\n Du hast %u mal in Knast beim %u mal wird dein Account gebannt\r\n|cffff0000!!! ACHTUNG - WICHTIG - WARNUNG !!!]', NULL, NULL, NULL, NULL, '|cffff0000[!!! ВНИМАНИЕ - ВАЖНО - ВНИМАНИЕ !!!\r\n Ваш персонаж уже %u раз был в тюрьме. Через %u раз(а) ваш аккаунт будет забанен!\r\n|cffff0000!!! ВНИМАНИЕ - ВАЖНО - ВНИМАНИЕ !!!]'),
   (973, 'Max. jailtimes reached!', NULL, 'Nombre maximum d\'Jails atteint!', 'Maximale Anzahl an Jails erreicht!', NULL, NULL, NULL, NULL, 'Максимальное количество отсидок достигнуто!'),
   (974, 'Robotron', NULL, 'Robotron', 'Robotron', NULL, NULL, NULL, NULL, 'Robotron'),
   (975, 'Your jail status was reset to 0', NULL, 'Votre statut a été Jail à 0 gesatzt', 'Dein Jail status wurde auf 0 zurück gesatzt', NULL, NULL, NULL, NULL, 'Ваша статистика тюрем сброшена на 0');

DELETE FROM `config` WHERE `name` LIKE 'Jail.%';
INSERT INTO `config` VALUES ('Jail.MaxJails', '3', '3', null, '3', 'Jail times when the char will be deleted or banned.');
INSERT INTO `config` VALUES ('Jail.MaxDuration', '672', '672', null, '672', 'Maximum jail duration in hours.');
INSERT INTO `config` VALUES ('Jail.MinReason', '25', '25', null, '25', 'Minimum character length of the reason.');
INSERT INTO `config` VALUES ('Jail.WarnPlayer', '1', '1', null, '1', 'Warn player every login if Jail.MaxJails is nearly reached.');
INSERT INTO `config` VALUES ('Jail.Amnestie', '180', '180', null, '180', '');
INSERT INTO `config` VALUES ('Jail.Tele.Alliance.X', null, '-8673.43', null, '-8673,43', '');
INSERT INTO `config` VALUES ('Jail.Tele.Alliance.Y', null, '631.795', null, '631,795', '');
INSERT INTO `config` VALUES ('Jail.Tele.Alliance.Z', null, '96.9406', null, '96,9406', '');
INSERT INTO `config` VALUES ('Jail.Tele.Alliance.O', null, '2.1785', null, '2,1785', '');
INSERT INTO `config` VALUES ('Jail.Tele.Alliance.Map', '0', '0', null, '0', '');
INSERT INTO `config` VALUES ('Jail.Tele.Horde.X', null, '2179.85', null, '2179,85', '');
INSERT INTO `config` VALUES ('Jail.Tele.Horde.Y', null, '-4763.96', null, '-4763,96', '');
INSERT INTO `config` VALUES ('Jail.Tele.Horde.Z', null, '54.911', null, '54,911', '');
INSERT INTO `config` VALUES ('Jail.Tele.Horde.O', null, '4.44216', null, '4,44216', '');
INSERT INTO `config` VALUES ('Jail.Tele.Horde.Map', '1', '1', null, '1', '');
INSERT INTO `config` VALUES ('Jail.Ban', null, null, '0', null, 'Ban acc if maximum jailtimes is reached.');
INSERT INTO `config` VALUES ('Jail.Radius', '10', '10', null, '10', 'Radius in which a jailed char can walk.');
