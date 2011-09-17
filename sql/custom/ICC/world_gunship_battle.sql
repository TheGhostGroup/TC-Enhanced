-- ScriptNames Updates
UPDATE `creature_template` SET `ScriptName` = 'npc_muradin_gunship' WHERE `entry` = 36948;
UPDATE `creature_template` SET `ScriptName` = 'npc_skybreaker_rifleman' WHERE `entry` = 36969;

UPDATE `creature_template` SET `ScriptName` = 'npc_saurfang_gunship' WHERE `entry` = 36939;
UPDATE `creature_template` SET `ScriptName` = 'npc_korkron_axethrower' WHERE `entry` = 36968;
UPDATE `creature_template` SET `ScriptName` = 'npc_korkron_primalist' WHERE `entry` = 37030;

UPDATE `creature_template` SET `ScriptName` = 'npc_zafod_boombox' WHERE `entry` = 37184;
UPDATE `creature_template` SET `ScriptName` = 'npc_gunship_cannon' WHERE `entry` = 36838;
UPDATE `creature_template` SET `ScriptName` = 'npc_gunship_cannon' WHERE `entry` = 36839;

-- SpellScripts
DELETE FROM `spell_script_names` WHERE spell_id IN (69487, 69188, 70713);
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES
(69487, "spell_icc_overheat"),
(69188, "spell_icc_rocket_pack"),
(70713, "spell_icc_remove_rocket_pack");

-- Templates updates
UPDATE `creature_template` SET `exp` = 2, `VehicleId` = 554, `RegenHealth` = 0, `spell1` = 69399, `spell2` = 69401 WHERE entry IN (36838, 36839);

-- Creature template Addon
DELETE FROM creature_template_addon WHERE entry IN (37184, 36948, 36939);
INSERT INTO `creature_template_addon` (`entry`, `path_id`, `mount`, `bytes1`, `bytes2`, `emote`, `auras`) VALUES
(37184, 0, 0, 0, 0, 0, '70348 69188'), -- Zafod Boombox (still unsure about 70348)
(36948, 0, 0, 0, 0, 0, '69637'), -- Muradin Bronzebeard's Battle Fury
(36939, 0, 0, 0, 0, 0, '69637'); -- High Overlord Saurfang's Battle Fury

-- Vehicle spellclick
DELETE FROM `npc_spellclick_spells` WHERE `npc_entry` IN (36838, 36839);
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `quest_start`, `quest_start_active`, `quest_end`, `cast_flags`, `aura_required`, `aura_forbidden`, `user_type`) VALUES
(36838, 70510, 0, 0, 0, 1, 0, 0, 0),
(36839, 70510, 0, 0, 0, 1, 0, 0, 0);

-- Linked spells
DELETE FROM `spell_linked_spell` WHERE `spell_trigger` = 71193 AND `spell_effect` = -71188;
DELETE FROM `spell_linked_spell` WHERE `spell_trigger` = 71195 AND `spell_effect` = -71193;
DELETE FROM `spell_linked_spell` WHERE `spell_trigger` = 68645 AND `spell_effect` = 69193;
DELETE FROM `spell_linked_spell` WHERE `spell_trigger` = 68645 AND `spell_effect` = 69188;
DELETE FROM `spell_linked_spell` WHERE `spell_trigger` = 69193 AND `spell_effect` = 69192;
INSERT INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `comment`) VALUES
(71193, -71188, 0, 'Gunship Battle - Veteran removes Experimented'),
(71195, -71193, 0, 'Gunship Battle - Elite removes Veteran'),
(68645, 69188, 0, 'Gunship Battle - Fix visual for Rocket Pack'),
(68645, 69193, 0, 'Gunship Battle - Fix visual for Rocket Pack'),
(68645, 68721, 0, 'Gunship Battle - Fix visual for Rocket Pack'),
(69193, 69192, 0, 'Gunship Battle - Cast Rocket Burst when landing with Rocket Pack');

-- Add spell conditions for 69705 (Below Zero)
SET @SPELL := 69705;
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=13 AND `SourceEntry`=@SPELL;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`,`SourceGroup`,`SourceEntry`,`ElseGroup`,`ConditionTypeOrReference`,`ConditionValue1`,`ConditionValue2`,`ConditionValue3`,`ErrorTextId`,`ScriptName`,`Comment`) VALUES
(13,0,@SPELL,0,18,1,36838,0,0,'','Gunship Battle - Spell 69705 (Below Zero) target creature 36838'),
(13,0,@SPELL,0,18,1,36839,0,0,'','Gunship Battle - Spell 69705 (Below Zero) target creature 36839');

-- Battle Standards Spells
DELETE FROM `creature_template_addon` WHERE `entry` IN (37044, 37041);
SET @KORKRON := 37044; -- Kor'kron Battle Standard
SET @SKYBREAKER := 37041; -- Skybreaker Battle Standard
SET @HSPELL := 69809;
SET @ASPELL := 69808;
UPDATE `creature_template` SET `AIName`='SmartAI' WHERE `entry` IN (@KORKRON, @SKYBREAKER);
DELETE FROM `smart_scripts` WHERE `source_type`=0 AND `entryorguid` IN (@KORKRON, @SKYBREAKER);
INSERT INTO `smart_scripts` (`entryorguid`,`source_type`,`id`,`link`,`event_type`,`event_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action_type`,`action_param1`,`action_param2`,`action_param3`,`action_param4`,`action_param5`,`action_param6`,`target_type`,`target_param1`,`target_param2`,`target_param3`,`target_x`,`target_y`,`target_z`,`target_o`,`comment`) VALUES
(@KORKRON   ,0,0,0,1,0,100,1,1000,1000,1000,1000,11,@HSPELL,2,0,0,0,0,1,0,0,0,0,0,0,0, 'Kor''kron Battle Standard - On spawn & reset - Cast spell 69809'),
(@SKYBREAKER,0,0,0,1,0,100,1,1000,1000,1000,1000,11,@ASPELL,2,0,0,0,0,1,0,0,0,0,0,0,0, 'Skybreaker Battle Standard - On spawn & reset - Cast spell 69808');

-- Texts
SET @MURADIN := 36948;
SET @OVERLORD := 36939;
SET @PRIMALIST := 37030;
SET @INVOKER := 37033;
SET @DEFENDER := 37032;
SET @VINDICATOR := 37003;
SET @SORCERER := 37026;
SET @PROTECTOR := 36998;
SET @SOUNDID := 0; -- Still have to find them (boring part)
DELETE FROM `creature_text` WHERE `entry` IN (36948, 36939, 37030, 37033, 37032, 37003, 37026, 36998);
INSERT INTO `creature_text` (`entry`,`groupid`,`id`,`text`,`type`,`language`,`probability`,`emote`,`duration`,`sound`,`comment`) VALUES
(@MURADIN,  0, 0, 'Fire up the engines! We got a meetin'' with destiny, lads!', 14, 0, 100, 0, 0, @SOUNDID, 'Muradin Bronzebeard - SAY_INTRO_ALLIANCE_0'),
(@MURADIN,  1, 0, 'Hold on to yer hats!', 14, 0, 100, 0, 0, @SOUNDID, 'Muradin Bronzebeard - SAY_INTRO_ALLIANCE_1'),
(@MURADIN,  2, 0, 'What in the world is that? Grab me spyglass, crewman!', 14, 0, 100, 0, 0, @SOUNDID, 'Muradin Bronzebeard - SAY_INTRO_ALLIANCE_2'),
(@MURADIN,  3, 0, 'By me own beard! HORDE SAILIN'' IN FAST ''N HOT!', 14, 0, 100, 0, 0, @SOUNDID, 'Muradin Bronzebeard - SAY_INTRO_ALLIANCE_3'),
(@MURADIN,  4, 0, 'EVASIVE ACTION! MAN THE GUNS!', 14, 0, 100, 0, 0, @SOUNDID, 'Muradin Bronzebeard - SAY_INTRO_ALLIANCE_4'),
(@MURADIN,  5, 0, 'Cowardly dogs! Ye blindsided us!', 14, 0, 100, 0, 0, @SOUNDID, 'Muradin Bronzebeard - SAY_INTRO_ALLIANCE_5'),
(@MURADIN,  6, 0, 'Not me battle? I dunnae who ye? think ye are, mister, but I got a score to settle with Arthas and yer not gettin'' in me way! FIRE ALL GUNS! FIRE! FIRE!', 14, 0, 100, 0, 0, @SOUNDID, 'Muradin Bronzebeard - SAY_INTRO_ALLIANCE_7'),
(@MURADIN,  7, 0, 'Move yer jalopy or we''ll blow it out of the sky, orc! The Horde''s got no business here!', 14, 0, 100, 0, 0, @SOUNDID, 'Muradin Bronzebeard - SAY_INTRO_HORDE_3'),
(@MURADIN,  8, 0, 'What''s this then?! Ye won''t be takin'' this son o'' Ironforge''s vessel without a fight!.', 14, 0, 100, 0, 0, @SOUNDID, 'Muradin Bronzebeard - SAY_BOARDING_SKYBREAKER_1'),
(@MURADIN,  9, 0, 'Marines, Sergeants, attack!', 14, 0, 100, 0, 0, @SOUNDID, 'Muradin Bronzebeard - SAY_BOARDING_ORGRIMS_HAMMER_0'),
(@MURADIN, 10, 0, 'Riflemen, shoot faster!', 14, 0, 100, 0, 0, @SOUNDID, 'Muradin Bronzebeard - SAY_NEW_RIFLEMEN_SPAWNED'),
(@MURADIN, 11, 0, 'Mortar team, reload!', 14, 0, 100, 0, 0, @SOUNDID, 'Muradin Bronzebeard - SAY_NEW_MORTAR_TEAM_SPAWNED'),
(@MURADIN, 12, 0, 'We''re taking hull damage, get a sorcerer out here to shut down those cannons!', 14, 0, 100, 0, 0, @SOUNDID, 'Muradin Bronzebeard - SAY_NEW_MAGE_SPAWNED'),
(@MURADIN, 13, 0, 'Don''t say I didn''t warn ya, scoundrels! Onward, brothers and sisters!', 14, 0, 100, 0, 0, @SOUNDID, 'Muradin Bronzebeard - SAY_ALLIANCE_VICTORY'),
(@MURADIN, 14, 0, 'Captain Bartlett, get us out of here! We''re taken too much damage to stay afloat!', 14, 0, 100, 0, 0, @SOUNDID, 'Muradin Bronzebeard - SAY_ALLIANCE_DEFEAT'),
(@OVERLORD, 0, 0, 'Rise up, sons and daughters of the Horde! Today we battle a hated enemy of the Horde! LOK''TAR OGAR! Kor''kron, take us out!', 14, 0, 100, 0, 0, @SOUNDID, 'High Overlord Saurfang - SAY_INTRO_HORDE_0'),
(@OVERLORD, 1, 0, 'What is that?! Something approaching in the distance!', 14, 0, 100, 0, 0, @SOUNDID, 'High Overlord Saurfang - SAY_INTRO_HORDE_1'),
(@OVERLORD, 2, 0, 'ALLIANCE GUNSHIP! ALL HANDS ON DECK!', 14, 0, 100, 0, 0, @SOUNDID, 'High Overlord Saurfang - SAY_INTRO_HORDE_2'),
(@OVERLORD, 3, 0, 'You will know our business soon! KOR''KRON, ANNIHILATE THEM!', 14, 0, 100, 0, 0, @SOUNDID, 'High Overlord Saurfang - SAY_INTRO_HORDE_4'),
(@OVERLORD, 4, 0, 'Reavers, Sergeants, attack!', 14, 0, 100, 0, 0, @SOUNDID, 'High Overlord Saurfang - SAY_BOARDING_SKYBREAKER_0'),
(@OVERLORD, 5, 0, 'You DARE board my ship? Your death will come swiftly.', 14, 0, 100, 0, 0, @SOUNDID, 'High Overlord Saurfang - SAY_BOARDING_ORGRIMS_HAMMER_1'),
(@OVERLORD, 6, 0, 'Axethrowers, hurl faster!', 14, 0, 100, 0, 0, @SOUNDID, 'High Overlord Saurfang - SAY_NEW_AXETHROWER_SPAWNED'),
(@OVERLORD, 7, 0, 'Rocketeers, reload!', 14, 0, 100, 0, 0, @SOUNDID, 'High Overlord Saurfang - SAY_NEW_ROCKETEERS_SPAWNED'),
(@OVERLORD, 8, 0, 'We''re taking hull damage, get a battle-mage out here to shut down those cannons!', 14, 0, 100, 0, 0, @SOUNDID, 'High Overlord Saurfang - SAY_NEW_BATTLE_MAGE_SPAWNED'),
(@OVERLORD, 9, 0, 'The Alliance falter. Onward to the Lich King!', 14, 0, 100, 0, 0, @SOUNDID, 'High Overlord Saurfang - SAY_HORDE_VICTORY'),
(@OVERLORD,10, 0, 'Damage control! Put those fires out! You haven''t seen the last of the Horde!', 14, 0, 100, 0, 0, @SOUNDID, 'High Overlord Saurfang - SAY_HORDE_DEFEAT'),
(@PRIMALIST,0, 0, 'Thank the spirits for you, brothers and sisters. The Skybreaker has already left. Quickly now, to Orgrim''s Hammer! If you leave soon, you may be able to catch them.', 12, 0, 100, 0, 0, @SOUNDID, 'Kor''kron Primalist - SAY_FIRST_SQUAD_RESCUED_HORDE_0'),
(@INVOKER,  0, 0, 'This should be helpin''ya!', 12, 0, 100, 0, 0, @SOUNDID, 'Kor''kron Invoker - SAY_FIRST_SQUAD_RESCUED_HORDE_1'),
(@INVOKER,  1, 0, '%s summons a Kor''kron Battle Standard.', 12, 0, 100, 0, 0, @SOUNDID, 'Kor''kron Invoker - SAY_SUMMON_BATTLE_STANDARD'),
(@DEFENDER, 0, 0, 'Aka''Magosh, brave warriors. The alliance is in great number here.', 12, 0, 100, 0, 0, @SOUNDID, 'Kor''kron Defender - SAY_SECOND_SQUAD_RESCUED_HORDE_0'),
(@DEFENDER, 1, 0, 'Captain Saurfang will be pleased to see you aboard Orgrim''s Hammer. Make haste, we will secure the area until you are ready for take-off.', 12, 0, 100, 0, 0, @SOUNDID, 'Kor''kron Defender - SAY_SECOND_SQUAD_RESCUED_HORDE_1'),
(@VINDICATOR, 0, 0, 'Thank goodness you arrived when you did, heroes. Orgrim''s Hammer has already left. Quickly now, to The Skybreaker! If you leave soon, you may be able to catch them.', 12, 0, 100, 0, 0, @SOUNDID, 'Skybreaker Vindicator - SAY_FIRST_SQUAD_RESCUED_ALLIANCE_0'),
(@SORCERER, 0, 0, 'This ought to help!', 12, 0, 100, 0, 0, @SOUNDID, 'Skybreaker Sorcerer - SAY_FIRST_SQUAD_RESCUED_ALLIANCE_1'),
(@SORCERER, 1, 0, '%s summons a Skybreaker Battle Standard.', 16, 0, 100, 0, 0, @SOUNDID, 'Skybreaker Sorcerer - SAY_SUMMON_BATTLE_STANDARD'),
(@PROTECTOR,0, 0, 'You have my thanks. We were outnumbered until you arrived.', 12, 0, 100, 0, 0, @SOUNDID, 'Skybreaker Protector - SAY_SECOND_SQUAD_RESCUED_ALLIANCE_0'),
(@PROTECTOR,1, 0, 'Captain Muradin is waiting aboard The Skybreaker. We''ll secure the area until you are ready for take off.', 12, 0, 100, 0, 0, @SOUNDID, 'Skybreaker Protector - SAY_SECOND_SQUAD_RESCUED_ALLIANCE_1'),
(@PROTECTOR,2, 0, 'Skybreaker infantry, hold position!', 12, 0, 100, 0, 0, @SOUNDID, 'Skybreaker Protector - SAY_SECOND_SQUAD_RESCUED_ALLIANCE_2');