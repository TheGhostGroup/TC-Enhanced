# Copyright (C) 2008-2011 TrinityCore <http://www.trinitycore.org/>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

set(scripts_STAT_SRCS
  ${scripts_STAT_SRCS}
  Custom/npc_teleguy.cpp
  Custom/npc_wintergrasp_honor_vendor.cpp
  Custom/event_npc.cpp
  Custom/npc_lottery.cpp
  Custom/duel_reset.cpp
  Custom/argent_tournament.cpp
  Custom/beastmaster.cpp
  Custom/npcbuff.cpp
  Custom/city_conquest_boss.cpp
  Custom/city_conquest_generic.cpp
  Custom/city_conquest_portal.cpp
  Custom/city_conquest_spawner.cpp
  Custom/professionnpc.cpp
  Custom/script_bot_giver.cpp
  Custom/levelnpc.cpp
)

message("  -> Prepared: Custom")
