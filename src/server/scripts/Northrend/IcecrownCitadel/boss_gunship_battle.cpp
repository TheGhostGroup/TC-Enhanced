/* Gunship Battle
 * Dev start : 29 / 05 / 2011
 * TODO List :
 * - [COMPLETE] Fix Below Zero to freeze only cannons.
     -> http://www.trinitycore.org/f/topic/3269-complete-gunship-battle-sai/page__gopid__19479#entry19479
 * - Encounter frames : the enemy one is the first one. They both are supposed to be friendly to 
        the player for him not to dot them, nor AoE, and should not be selectable.
        Hencemore, they appear when the enemy ship is boarding yours.
 * - Riflemen are spawned directly on the other side of the space separating Players VS NPCs. 4 of them
 * - Mortar soldiers and Rocketeers are spawned on the extremity of the ship. This can be clearly seen from videos. 2 of them.
 * - Rocket boots have an AoE damage when landing. Heroic only ?
 * - The targeting mark for rockets are cast when that spell is also cast, not when it hits the ground.
 * - When players win, the enemy zeppelin escapes VERY quickly
 * Notes:
 * - Rocketeers seem to be able to cast on their own ship. For their spell, take a look at SPELL_ROCKET_ARTILLERY_TARGET_ALLIANCE and SPELL_ROCKET_ARTILLERY_TARGET_HORDE (Script effect, SpellEffect.cpp, EffectScriptEffect(EffIndex index))
 * Reference videos:
   http://www.youtube.com/watch?v=ekjRGPawpas // Wyrm
   http://www.youtube.com/watch?v=Lr_nnAYZCyE // Wyrm (movement really far away)
   http://www.youtube.com/watch?v=3bIcqOdteac // Mages
 */

#include "ScriptPCH.h"
#include "icecrown_citadel.h"
#include "MapManager.h"
#include "Transport.h"
#include "Vehicle.h"
#include "Spell.h"

struct NPCsPositions
{
    uint32 npcId; // `creature_template`.`entry`
    // The position should be set offseted from the transport's position.
    Position position;
    uint32 faction;
};

// Note: -472.596f, 2466.8701f, 190.7371f, 6.204f MURADIN_BRONZEBEARD

// These NPCs have their positions on the enemy Ship, IF their faction is different from the team in instance
const NPCsPositions enemyShipNPCList[]=
{
    // Alliance NPCs
    {NPC_GB_MURADIN_BRONZEBEARD,           {0.0f, 0.0f, 0.0f, 0.0f}, ALLIANCE},
    {NPC_GB_SKYBREAKER,                    {0.0f, 0.0f, 0.0f, 0.0f}, ALLIANCE},
    {NPC_GB_SKYBREAKER_MORTAR_SOLDIER,     {0.0f, 0.0f, 0.0f, 0.0f}, ALLIANCE}, // 4x 25Men, 2x 10Men
    {NPC_GB_SKYBREAKER_RIFLEMAN,           {0.0f, 0.0f, 0.0f, 0.0f}, ALLIANCE}, // 8/6x 25Men, 4x 10Men

    // Horde NPCs
    {NPC_GB_HIGH_OVERLORD_SAURFANG,        {0.0f, 0.0f, 0.0f, 0.0f}, HORDE},
    {NPC_GB_ORGRIMS_HAMMER,                {0.0f, 0.0f, 0.0f, 0.0f}, HORDE},
    {NPC_GB_KORKRON_ROCKETEER,             {0.0f, 0.0f, 0.0f, 0.0f}, HORDE}, // 4x 25Men, 2x 10Men
    {NPC_GB_KORKRON_AXETHROWER,            {0.0f, 0.0f, 0.0f, 0.0f}, HORDE}, // 8/6x 25Men, 4x 10Men
};

// These NPCs have their positions on the friendly Ship, IF their faction is different from the team in instance
const NPCsPositions friendlyShipNPCList[]=
{
    // Alliance NPCs
    {NPC_GB_MURADIN_BRONZEBEARD,           {0.0f, 0.0f, 0.0f, 0.0f}, ALLIANCE},
    {NPC_GB_SKYBREAKER,                    {0.0f, 0.0f, 0.0f, 0.0f}, ALLIANCE},
    {NPC_GB_SKYBREAKER_MORTAR_SOLDIER,     {0.0f, 0.0f, 0.0f, 0.0f}, ALLIANCE}, // 4x 25Men, 2x 10Men
    {NPC_GB_SKYBREAKER_RIFLEMAN,           {0.0f, 0.0f, 0.0f, 0.0f}, ALLIANCE}, // 8/6x 25Men, 4x 10Men

    // Horde NPCs
    {NPC_GB_HIGH_OVERLORD_SAURFANG,        {0.0f, 0.0f, 0.0f, 0.0f}, HORDE},
    {NPC_GB_ORGRIMS_HAMMER,                {0.0f, 0.0f, 0.0f, 0.0f}, HORDE},
    {NPC_GB_KORKRON_ROCKETEER,             {0.0f, 0.0f, 0.0f, 0.0f}, HORDE}, // 4x 25Men, 2x 10Men
    {NPC_GB_KORKRON_AXETHROWER,            {0.0f, 0.0f, 0.0f, 0.0f}, HORDE}, // 8/6x 25Men, 4x 10Men
};

enum Spells
{
    // Cannon
    SPELL_OVERHEAT                    = 69487, // Triggers spell #69488 every 0.25s. It should consume 10 Energy but does not.
    SPELL_CANNON_BLAST                = 69399,
    SPELL_INCINERATING_BLAST          = 69401,

    // Auras
    SPELL_ON_ORGRIMS_HAMMERS_DECK     = 70121,
    SPELL_ON_SKYBREAKERS_DECK         = 70120,

    // Achievement spell required target
    SPELL_ACHIEVEMENT                 = 72959,

    // Rampart of Skulls NPCs Spells
    // Kor'kron Primalist
    SPELL_WRATH                       = 69968,
    SPELL_HEALING_TOUCH               = 69899,
    SPELL_REGROWTH                    = 69882,
    SPELL_REJUVENATION                = 69898,

    // Muradin Bronzebeard / High Overlord Saurfang
    SPELL_CLEAVE                      = 15284,
    SPELL_RENDING_THROW               = 70309,
    SPELL_TASTE_OF_BLOOD              = 69634,

    // Kor'kron Battle-mage & Skybreaker Sorcerer
    SPELL_BELOW_ZERO                  = 69705,

    // Experience spells
    SPELL_EXPERIENCED                 = 71188,
    SPELL_VETERAN                     = 71193,
    SPELL_ELITE                       = 71195,
    SPELL_DESPERATE_RESOLVE           = 69647,

    // Kor'kron Axethrower & Skybreaker Rifleman
    SPELL_HURL_AXE                    = 70161,
    SPELL_SHOOT                       = 70162,

    // Kor'kron Rocketeer & Skybreaker Mortar Soldier
    SPELL_ROCKET_ARTILLERY_MARKER     = 71371,
    SPELL_ROCKET_ARTILLERY_TRIGGERED  = 69679,
    SPELL_ROCKET_ARTILLERY_HORDE      = 69678,
    SPELL_ROCKET_ARTILLERY_ALLIANCE   = 70609,
    SPELL_EXPLOSION                   = 69680,
};

enum Events
{
    EVENT_INTRO_HORDE_0, // High Overlord Saurfang yells: Rise up, sons and daughters of the Horde! Today we battle a hated enemy of the Horde! LOK'TAR OGAR! Kor'kron, take us out!
    EVENT_INTRO_HORDE_1, // High Overlord Saurfang yells: What is that?! Something approaching in the distance! 
    EVENT_INTRO_HORDE_2, // High Overlord Saurfang yells: ALLIANCE GUNSHIP! ALL HANDS ON DECK!
    EVENT_INTRO_HORDE_3, // Muradin Bronzebeard yells: Move yer jalopy or we'll blow it out of the sky, orc! The Horde's got no business here!
    EVENT_INTRO_HORDE_4, // High Overlord Saurfang yells: You will know our business soon! KOR'KRON, ANNIHILATE THEM!

    EVENT_INTRO_ALLIANCE_0, // Muradin Bronzebeard yells: Fire up the engines! We got a meetin' with destiny, lads!
    EVENT_INTRO_ALLIANCE_1, // Muradin Bronzebeard yells: Hold on to yer hats!
    EVENT_INTRO_ALLIANCE_2, // Muradin Bronzebeard yells: What in the world is that? Grab me spyglass, crewman!
    EVENT_INTRO_ALLIANCE_3, // Muradin Bronzebeard yells: By me own beard! HORDE SAILIN' IN FAST 'N HOT!
    EVENT_INTRO_ALLIANCE_4, // Muradin Bronzebeard yells: EVASIVE ACTION! MAN THE GUNS!
    EVENT_INTRO_ALLIANCE_5, // Muradin Bronzebeard yells: Cowardly dogs! Ye blindsided us!
    EVENT_INTRO_ALLIANCE_6, // High Overlord Saurfang yells: This is not your battle, dwarf. Back down or we will be forced to destroy your ship.
    EVENT_INTRO_ALLIANCE_7, // Muradin Bronzebeard yells: Not me battle? I dunnae who ye think ye are, mister, but I got a score to settle with Arthas and yer not gettin' in me way! FIRE ALL GUNS! FIRE! FIRE!

    // Rampart of Skulls NPCs Events
    EVENT_WRATH,
    EVENT_HEAL,

    // First Squad Assisted
    EVENT_FIRST_SQUAD_ASSISTED_1,
    EVENT_FIRST_SQUAD_ASSISTED_2,

    // Shared experience events
    EVENT_EXPERIENCED,
    EVENT_VETERAN,
    EVENT_ELITE,

    // Kor'kron Axethrower & Skybreaker Rifleman
    EVENT_HURL_AXE,
    EVENT_SHOOT,

    // Muradin Bronzebeard & High Overlord Saurfang
    EVENT_RENDING_THROW,
    EVENT_TASTE_OF_BLOOD,

    // Misc (used in various NPCs)
    EVENT_SPAWN_MAGE,
    EVENT_RESPAWN_RIFLEMEN,
    EVENT_RESPAWN_ROCKETEER,
};

enum Texts
{
    // Kor'kron Primalist
    SAY_FIRST_SQUAD_RESCUED_HORDE_0  = 0,

    // Kor'kron Invoker
    SAY_FIRST_SQUAD_RESCUED_HORDE_1  = 0,

    // Kor'kron Defender
    SAY_SECOND_SQUAD_RESCUED_HORDE_0 = 0,
    SAY_SECOND_SQUAD_RESCUED_HORDE_1 = 1,

    // Skybreaker Vindicator
    SAY_FIRST_SQUAD_RESCUED_ALLIANCE_0  = 0,

    // Skybreaker Sorcerer
    SAY_FIRST_SQUAD_RESCUED_ALLIANCE_1  = 0,

    // Skybreaker Protector
    SAY_SECOND_SQUAD_RESCUED_ALLIANCE_0 = 0,
    SAY_SECOND_SQUAD_RESCUED_ALLIANCE_1 = 1,
    SAY_SECOND_SQUAD_RESCUED_ALLIANCE_2 = 2,
    
    // Kor'kron Invoker & Skybreaker Sorcerer
    SAY_SUMMON_BATTLE_STANDARD          = 1, 

    // -- These two are left to do
    // A screeching cry pierces the air above! (Widescreen Yellow Emote)
    // A Spire Frostwyrm lands just before Orgrim's Hammer. (Chat message)
    // --

    // Muradin Bronzebeard
    SAY_INTRO_ALLIANCE_0             = 0,
    SAY_INTRO_ALLIANCE_1             = 1,
    SAY_INTRO_ALLIANCE_2             = 2,
    SAY_INTRO_ALLIANCE_3             = 3,
    SAY_INTRO_ALLIANCE_4             = 4,
    SAY_INTRO_ALLIANCE_5             = 5,
    SAY_INTRO_ALLIANCE_7             = 6,
    SAY_INTRO_HORDE_3                = 7,
    SAY_BOARDING_SKYBREAKER_1        = 8,
    SAY_BOARDING_ORGRIMS_HAMMER_0    = 9,
    SAY_NEW_RIFLEMEN_SPAWNED         = 10,
    SAY_NEW_MORTAR_TEAM_SPAWNED      = 11,
    SAY_NEW_MAGE_SPAWNED             = 12,
    SAY_ALLIANCE_VICTORY             = 13,
    SAY_ALLIANCE_DEFEAT              = 14, // How will we handle that case ? Ie. the player loses 

    // High Overlord Saurfang
    SAY_INTRO_HORDE_0                = 0,
    SAY_INTRO_HORDE_1                = 1,
    SAY_INTRO_HORDE_2                = 2,
    SAY_INTRO_HORDE_4                = 3,
    SAY_BOARDING_SKYBREAKER_0        = 4,
    SAY_BOARDING_ORGRIMS_HAMMER_1    = 5,
    SAY_NEW_AXETHROWER_SPAWNED       = 6,
    SAY_NEW_ROCKETEERS_SPAWNED       = 7,
    SAY_NEW_BATTLE_MAGE_SPAWNED      = 8,
    SAY_HORDE_VICTORY                = 9,
    SAY_HORDE_DEFEAT                 = 10, // How will we handle that case ? Ie. the player loses  
};

enum Actions
{
    ACTION_SPAWN_MAGE,
    ACTION_RESPAWN_RIFLEMEN,
    ACTION_RESPAWN_ROCKETEERS,
};

enum Points
{
    POINT_FIRST_POS_MAGE_ALLIANCE        = 3711601,
    POINT_SECOND_POS_MAGE_ALLIANCE       = 3711602,
    POINT_BELOW_ZERO                     = 3711603,
};

/* ----------------------------------- Behavior : --------------------------------- */
// Transport* wut = instance->PrepareTransport(goEntry, period);
// wut = instance->SetTransportPosition(wut, position, goEntry);

void AddPassengers(Transport* t, uint32 teamInInstance, uint32 transportFaction)
{
    bool is25MenMap = (t->GetMap()->ToInstanceMap()->GetMaxPlayers() == 25);

    const NPCsPositions* list = (transportFaction == teamInInstance ? friendlyShipNPCList : enemyShipNPCList);

    while (list->npcId)
    {
        // Only spawn NPCs that have the same faction as the transport's
        // In fact the transport's faction will be set raw when calling the function,
        // like AddPassengers(skybreaker, instance->GetData(DATA_TEAM_IN_INSTANCE), ALLIANCE);
        if (list->faction == transportFaction)
            t->AddNPCPassenger(t->GetTransGUID(), list->npcId, list->position.m_positionX, list->position.m_positionY, list->position.m_positionZ, list->position.m_orientation, 100);
        ++list;
    }
}

/* ----------------------------------- Gunship Battle Itself ----------------------------------- */

/* transport script */
class transport_gunship : public TransportScript
{
    public:
        transport_gunship() : TransportScript("transport_gunship") { }

        void OnRelocate(Transport* transport, uint32 waypointId, uint32 mapId, float x, float y, float z)
        {
            sLog->outString("ICC::Gunship: Transport %s reached waypoint %u/%u. Position is X:%u, Y:%u, Z:%u.", GetName(), waypointId, transport->m_WayPoints.size(), x, y, z);
        }

        void OnAddPassenger(Transport* transport, Player* player)
        {
            if (InstanceScript* instance = transport->GetInstanceScript())
            {
                switch (instance->GetData(DATA_TEAM_IN_INSTANCE))
                {
                    case HORDE:
                        player->AddAura(SPELL_ON_ORGRIMS_HAMMERS_DECK, player);
                        break;
                    case ALLIANCE:
                        player->AddAura(SPELL_ON_SKYBREAKERS_DECK, player);
                        break;
                }
            }
        }

        void OnRemovePassenger(Transport* transport, Player* player)
        {
            player->RemoveAurasDueToSpell(SPELL_ON_ORGRIMS_HAMMERS_DECK);
            player->RemoveAurasDueToSpell(SPELL_ON_SKYBREAKERS_DECK);
        }
};

/* Muradin Bronzebeard */
class npc_muradin_gunship : public CreatureScript
{
    public:
        npc_muradin_gunship() : CreatureScript("npc_muradin_gunship") { }

        bool OnGossipHello(Player* player, Creature* pCreature)
        {
            InstanceScript* pInstance = pCreature->GetInstanceScript();
            if (pInstance && pInstance->GetBossState(DATA_GUNSHIP_EVENT) != DONE)
            {
                player->ADD_GOSSIP_ITEM(0, "My companions are all accounted for, Muradin. Let's go!", 631, 1001);
                // Some testing items - ALL MAY LEAD TO CRASH, TESTIN' PURPOSE ONLY
                player->ADD_GOSSIP_ITEM(0, "Spawn transport using GO_SKYBREAKER_FRIENDLY", 631, 1002);
                player->ADD_GOSSIP_ITEM(0, "Spawn transport using GO_SKYBREAKER_UNFRIENDLY", 631, 1003);
                player->ADD_GOSSIP_ITEM(0, "Spawn transport using GO_ORGRIMS_HAMMER_FRIENDLY", 631, 1004);
                player->ADD_GOSSIP_ITEM(0, "Spawn transport using GO_ORGRIMS_HAMMER_UNFRIENDLY", 631, 1005);

                player->SEND_GOSSIP_MENU(player->GetGossipTextId(pCreature), pCreature->GetGUID());
                return true;
            }

            return false;
        }

        bool OnGossipSelect(Player* player, Creature* pCreature, uint32 /*sender*/, uint32 action)
        {
            InstanceScript* instance = pCreature->GetInstanceScript();
            player->PlayerTalkClass->ClearMenus();
            player->CLOSE_GOSSIP_MENU();
            if (action == 1001) // Make the boat move
            {
                if (Transport* playersBoat = pCreature->GetTransport())
                {    
                    Map* tMap = player->GetMap();
                    pInstance->SetData(DATA_GUNSHIP_EVENT, IN_PROGRESS);
                    playersBoat->BuildStartMovePacket(tMap);
                    playersBoat->SetGoState(GO_STATE_ACTIVE);
                    playersBoat->Update(0);
                    playersBoat->SetUInt32Value(GAMEOBJECT_DYNAMIC, 0x10830010); // Seen in sniffs
                    playersBoat->SetFloatValue(GAMEOBJECT_PARENTROTATION + 3, 1.0f);
                   
                    // Transmit movement packet
                    for (Map::PlayerList::const_iterator itr = tMap->GetPlayers().begin(); itr != tMap->GetPlayers().end(); ++itr)
                        if (Player* pPlayer = itr->getSource())
                        {
                            UpdateData transData;
                            playersBoat->BuildCreateUpdateBlockForPlayer(&transData, pPlayer);
                            WorldPacket packet;
                            transData.BuildPacket(&packet);
                            pPlayer->GetSession()->SendPacket(&packet);
                        }

                    // skybreaker->BuildStopMovePacket(tMap); // Nope, we're testin'
                }
                else
                    pCreature->MonsterYell("FUUUUU I'm not on my ship !", LANG_UNIVERSAL, 0);
            }
            else
            {
                if (action > 1001 && action < 1006)
                {
                    uint32 goEntry;
                    switch (action)
                    {
                        case 1002:
                            goEntry = 201811;
                            break;
                        case 1003:
                            goEntry = 201580;
                            break;
                        case 1004:
                            goEntry = 201812;
                            break;
                        case 1005:
                            goEntry = 201581;
                            break;
                    }

                    const GameObjectTemplate* goInfo = sObjectMgr->GetGameObjectTemplate(goEntry);
                    if (!goInfo)
                    {
                        pCreature->MonsterYell("gameobject_template missing", LANG_UNIVERSAL, 0);
                        sLog->outErrorDb("Transport ID: %u, Name: %s, will not be loaded, gameobject_template is missing", goEntry, goInfo->name.c_str());
                        return false;
                    }
                    Transport* t = new Transport(51584, goInfo->ScriptId);

                    std::set<uint32> mapsUsed;
                    if (!t->GenerateWaypoints(goInfo->moTransport.taxiPathId, mapsUsed))
                    {
                        sLog->outErrorDb("Transport (path id %u) path size = 0. Transport ignored, check DBC files or the gameobject's data0 field.", goInfo->moTransport.taxiPathId);
                        delete t;
                        pCreature->MonsterYell("Path size = 0", LANG_UNIVERSAL, 0);
                        return false;
                    }

                    uint32 transportLowGuid = sObjectMgr->GenerateLowGuid(HIGHGUID_MO_TRANSPORT);

                    // Creates the Gameobject
                    if (!t->Create(transportLowGuid, goEntry, t->m_WayPoints[0].mapid, t->m_WayPoints[0].x, t->m_WayPoints[0].y, t->m_WayPoints[0].z, 0.0f, 0, 0))
                    {
                        delete t;
                        pCreature->MonsterYell("Could not create the transport", LANG_UNIVERSAL, 0);
                        return false;
                    }

                    t->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_IN_USE);
                    t->SetGoState(GO_STATE_READY);
                    Map* tMap = player->GetMap();
                    t->SetMap(tMap);
                    t->AddToWorld();

                    // Transmit creation packet to all players on map
                    for (Map::PlayerList::const_iterator itr = tMap->GetPlayers().begin(); itr != tMap->GetPlayers().end(); ++itr)
                        if (Player* pPlayer = itr->getSource())
                        {
                            UpdateData transData;
                            t->BuildCreateUpdateBlockForPlayer(&transData, pPlayer);
                            WorldPacket packet;
                            transData.BuildPacket(&packet);
                            pPlayer->SendDirectMessage(&packet);
                        }

                    sMapMgr->m_Transports.insert(playersBoat);
                    t->Update(1);
                    t->BuildStopMovePacket(tMap);

                    pCreature->MonsterYell("Transport was spawned", LANG_UNIVERSAL, 0);
                }
            }

            return true;
        }

        struct npc_muradin_gunshipAI : public ScriptedAI
        {
            npc_muradin_gunshipAI(Creature *creature) : ScriptedAI(creature),
                _instance(creature->GetInstanceScript())
            {
            }

            void DoAction(int32 const action)
            {
                switch (action)
                {
                    case ACTION_SPAWN_MAGE:
                        events.ScheduleEvent(EVENT_SPAWN_MAGE, 100);
                        break;
                    case ACTION_RESPAWN_RIFLEMEN:
                        events.ScheduleEvent(EVENT_RESPAWN_RIFLEMEN, 100);
                        break;
                    case ACTION_RESPAWN_ROCKETEERS:
                        events.ScheduleEvent(EVENT_RESPAWN_ROCKETEER, 100);
                        break;
                }
            }

            void UpdateAI(const uint32 diff)
            {
                if (UpdateVictim())
                {
                    if (me->getVictim()->HasAura(SPELL_ON_ORGRIMS_HAMMERS_DECK))
                    {
                        if (!me->getVictim()->IsWithinDistInMap(me, 50.0f, false)) // Todo: Fix the distance
                            EnterEvadeMode();
                        else
                            events.ScheduleEvent(EVENT_RENDING_THROW, 100);
                    }
                }

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_RENDING_THROW:
                            if (UpdateVictim())
                                if (me->getVictim()->IsWithinDistInMap(me, 50.0f, false)) // Todo: Fix the distance
                                {
                                    DoCastVictim(SPELL_RENDING_THROW);
                                    events.ScheduleEvent(EVENT_RENDING_THROW, 5000); // Todo: fix the timer
                                }
                                else
                                    events.CancelEvent(EVENT_RENDING_THROW);
                            break;
                        case EVENT_TASTE_OF_BLOOD:
                            DoCast(me, SPELL_TASTE_OF_BLOOD);
                            events.ScheduleEvent(EVENT_TASTE_OF_BLOOD, 15000); // Todo: fix the timer
                            break;
                        case EVENT_SPAWN_MAGE:
                            Talk(SAY_NEW_MAGE_SPAWNED);
                            break;
                        case EVENT_RESPAWN_RIFLEMEN:
                            Talk(SAY_NEW_RIFLEMEN_SPAWNED);
                            break;
                        case EVENT_RESPAWN_ROCKETEER:
                            Talk(SAY_NEW_ROCKETEERS_SPAWNED);
                            break;
                    }
                }

                if (!me->GetCurrentSpell(CURRENT_MELEE_SPELL))
                    DoCastVictim(SPELL_CLEAVE);
            }

        private:
            EventMap events;
            InstanceScript* _instance;
            bool canCleave;
        };

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_muradin_gunshipAI(pCreature);
        }

        private:
            InstanceScript* pInstance;
            Transport* playersBoat;
};

/* Zafod boombox */
class npc_zafod_boombox : public CreatureScript
{
    public:
        npc_zafod_boombox() : CreatureScript("npc_zafod_boombox") { }

        bool OnGossipHello(Player* pPlayer, Creature* pCreature)
        {
            // Maybe this isn't blizzlike but I can't find any spell in the DBCs
            if (pPlayer->GetItemCount(49278, false) == 0)
                pPlayer->ADD_GOSSIP_ITEM(0, "Yeah, I'm sure safety is your top priority. Give me a rocket pack.", 631, 1);
            pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* pCreature, uint32 /*sender*/, uint32 action)
        {
            player->PlayerTalkClass->ClearMenus();
            player->CLOSE_GOSSIP_MENU();
            if (action == 1)
            {
                // Seurity, this shouldn't happen. Maybe useless.
                uint32 curItemCount = player->GetItemCount(49278, false);
                if (curItemCount >= 1)
                {
                    pCreature->MonsterWhisper("You already have my rocket pack!", player->GetGUIDLow());
                    return false;
                }

                ItemPosCountVec dest;
                uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 49278, 1);
                if (msg == EQUIP_ERR_OK)
                {
                    Item* item = player->StoreNewItem(dest, 49278, true);
                    player->SendNewItem(item, 1, true, false);
                }
                else
                {
                    pCreature->MonsterWhisper("You don't have any empty space for my rocket pack!", player->GetGUIDLow());
                    return false;
                }
            }

            return true;
        }
};

/* Gunship Cannon (Horde / Ally) */
class npc_gunship_cannon : public CreatureScript
{
    public:
        npc_gunship_cannon() : CreatureScript("npc_gunship_cannon") { }

        struct npc_gunship_cannonAI : public ScriptedAI
        {
            npc_gunship_cannonAI(Creature *creature) : ScriptedAI(creature) { }

            void SpellHit(Unit* caster, SpellEntry const* spellEntry)
            {
                if (spellEntry->Id == SPELL_BELOW_ZERO)
                    me->GetVehicleKit()->RemoveAllPassengers();
            }

            void UpdateAI(const uint32 diff)
            {
                if (Powers powerType = me->getPowerType())
                    if (me->GetPower(powerType) == me->GetMaxPower(powerType))
                        DoCast(me, SPELL_OVERHEAT); // TODO : fix infinite cooldown on cannon abilities
            }
        };
};

/* Kro'kron Axethrower */
class npc_korkron_axethrower : public CreatureScript
{
    public:
        npc_korkron_axethrower() : CreatureScript("npc_korkron_axethrower") { }

        struct npc_korkron_axethrowerAI : public Scripted_NoMovementAI
        {
            npc_korkron_axethrowerAI(Creature *creature) : Scripted_NoMovementAI(creature)
            {
                bool isAllianceInInstance = (creature->GetInstanceScript()->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE);
                Reset();
            }

            void Reset()
            {
                ScriptedAI::Reset();
                desperated = false;
                me->RemoveAurasDueToSpell(SPELL_EXPERIENCED);
                me->RemoveAurasDueToSpell(SPELL_ELITE);
                me->RemoveAurasDueToSpell(SPELL_VETERAN);

                if (!isAllianceInInstance) // Of course, it should never return here.
                    return;
                events.ScheduleEvent(EVENT_EXPERIENCED, urand(19000, 21000)); // ~20 sec
                events.ScheduleEvent(EVENT_VETERAN, urand(39000, 41000));     // ~40 sec
                events.ScheduleEvent(EVENT_ELITE, urand(59000, 61000));       // ~60 sec
            }

            void DamageTaken(Unit* /*attacker*/, uint32 /*damage*/)
            {
                if (isAllianceInInstance && me->GetHealthPct() < 20.0f && !desperated)
                {
                    desperated = true;
                    DoCast(me, SPELL_DESPERATE_RESOLVE);
                }
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (!me->HasUnitState(UNIT_STAT_CASTING))
                    DoCast(me->getVictim(), SPELL_HURL_AXE);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_EXPERIENCED:
                            DoCast(me, SPELL_EXPERIENCED);
                            break;
                        case EVENT_VETERAN:
                            DoCast(me, SPELL_VETERAN);
                            break;
                        case EVENT_ELITE:
                            DoCast(me, SPELL_ELITE);
                            break;
                        default:
                            break;
                    }
                }
            }

            private:
                EventMap events;
                bool isAllianceInInstance;
                bool desperated;
        };

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_korkron_axethrowerAI(pCreature);
        }
};

/* Skybreaker Rifleman */
class npc_skybreaker_rifleman : public CreatureScript
{
    public:
        npc_skybreaker_rifleman() : CreatureScript("npc_skybreaker_rifleman") { }

        struct npc_skybreaker_riflemanAI : public Scripted_NoMovementAI
        {
            npc_skybreaker_riflemanAI(Creature *creature) : Scripted_NoMovementAI(creature)
            {
                bool isHordeInInstance = (creature->GetInstanceScript()->GetData(DATA_TEAM_IN_INSTANCE) == HORDE);
                Reset();
            }

            void Reset()
            {
                ScriptedAI::Reset();
                desperated = false;
                me->RemoveAurasDueToSpell(SPELL_EXPERIENCED);
                me->RemoveAurasDueToSpell(SPELL_ELITE);
                me->RemoveAurasDueToSpell(SPELL_VETERAN);

                if (!isHordeInInstance) // Same as his Alliance copy
                    return;
                events.ScheduleEvent(EVENT_EXPERIENCED, urand(19000, 21000)); // ~20 sec
                events.ScheduleEvent(EVENT_VETERAN, urand(39000, 41000));     // ~40 sec
                events.ScheduleEvent(EVENT_ELITE, urand(59000, 61000));       // ~60 sec
            }

            void DamageTaken(Unit* /*attacker*/, uint32 /*damage*/)
            {
                if (isHordeInInstance && me->GetHealthPct() < 20.0f && !desperated)
                {
                    desperated = true;
                    DoCast(me, SPELL_DESPERATE_RESOLVE);
                }
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (!me->HasUnitState(UNIT_STAT_CASTING))
                    DoCast(me->getVictim(), SPELL_SHOOT);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_EXPERIENCED:
                            DoCast(me, SPELL_EXPERIENCED);
                            break;
                        case EVENT_VETERAN:
                            DoCast(me, SPELL_VETERAN);
                            break;
                        case EVENT_ELITE:
                            DoCast(me, SPELL_ELITE);
                            break;
                        default:
                            break;
                    }
                }
            }

            private:
                EventMap events;
                bool isHordeInInstance;
                bool desperated;
        };

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_skybreaker_riflemanAI(pCreature);
        }
};

/* The Skybreaker */
class npc_the_skybreaker : public CreatureScript
{
    public:
        npc_the_skybreaker() : CreatureScript("npc_the_skybreaker") { }

        struct npc_the_skybreakerAI : public ScriptedAI
        {
            npc_the_skybreakerAI(Creature* creature) : ScriptedAI(creature),
                _instance(creature->GetInstanceScript())
            {
            }

            void JustDied()
            {
                if (_instance->GetBossState(DATA_GUNSHIP_EVENT) == IN_PROGRESS)
                {
                    _instance->SendEncounterUnit(ENCOUNTER_FRAME_REMOVE, me);
                    _instance->SetBossState(DATA_GUNSHIP_EVENT, (_instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE ? FAIL : DONE));
                }
            }

            void UpdateAI(const uint32 diff)
            {
                // Does not enter combat
                if (_instance->GetBossState(DATA_GUNSHIP_EVENT) != IN_PROGRESS)
                    return;
            }

        private:
            EventMap _events;
            InstanceScript* _instance;
        };

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_the_skybreakerAI(pCreature);
        }
};

// This variable is only used by rocketeers, this is a hack
struct mortarMarksLoc
{
    uint32 durationBeforeRefreshing;
    Position location;
};

class npc_korkron_rocketeer : public CreatureScript
{
    public:
        npc_korkron_rocketeer() : CreatureScript("npc_korkron_rocketeer") { }

        struct npc_korkron_rocketeerAI : public ScriptedAI
        {
            npc_korkron_rocketeerAI(Creature *creature) : ScriptedAI(creature) { }

            void UpdateAI(const uint32 diff)
            {
                if (UpdateVictim())
                {
                    Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true, SPELL_ON_SKYBREAKERS_DECK);
                    Position pos;
                    target->GetPosition(&pos);
                    mortarMarksLoc markPos;
                    markPos.durationBeforeRefreshing = 5000;
                    markPos.location = pos;
                    marks.push_back(markPos);
                    me->CastSpell(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), SPELL_ROCKET_ARTILLERY_TRIGGERED, true);
                }

                // Hacky, spell system is not blizzlike
                // May also lead to crashes ?
                for (std::list<mortarMarksLoc>::iterator itr = marks.begin(); itr != marks.end(); ++itr)
                {
                    if ((*itr).durationBeforeRefreshing == 5000)
                        me->CastSpell((*itr).location.GetPositionX(), (*itr).location.GetPositionY(), (*itr).location.GetPositionZ(), SPELL_ROCKET_ARTILLERY_MARKER, true);
                    (*itr).durationBeforeRefreshing -= diff;
                }
            }

            std::list<mortarMarksLoc> getMarksPositions() { return marks; }
            bool setMarks(std::list<mortarMarksLoc> newMarks) { marks = newMarks; }
        private:
            EventMap events;
            std::list<mortarMarksLoc> marks;
        };
};

/* ----------------------------------- Rampart of Skulls NPCs ----------------------------------- */

/* Kor'kron Primalist */
class npc_korkron_primalist: public CreatureScript
{
    public:
        npc_korkron_primalist() : CreatureScript("npc_korkron_primalist") { }
    
        struct npc_korkron_primalistAI : public ScriptedAI
        {
            npc_korkron_primalistAI(Creature *creature) : ScriptedAI(creature) 
            {
                instance = creature->GetInstanceScript();
            }

            void Reset()
            {
                ScriptedAI::Reset();
                events.ScheduleEvent(EVENT_WRATH, 10000); // TODO: Fix the timers
                events.ScheduleEvent(EVENT_HEAL, 20000); // TODO: Fix the timers
            }

            void UpdateAI(const uint32 diff)
            {
                if (!instance)
                    return;

                if (instance->GetData(DATA_TEAM_IN_INSTANCE) == HORDE)
                {
                    if (instance->GetData(DATA_FIRST_SQUAD_STATE) != DONE)
                    {
                        instance->SetData(DATA_FIRST_SQUAD_STATE, DONE);
                        events.ScheduleEvent(EVENT_FIRST_SQUAD_ASSISTED_1, 100);
                        events.ScheduleEvent(EVENT_FIRST_SQUAD_ASSISTED_2, 15000); // TODO : fix the timer
                    }
                }

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FIRST_SQUAD_ASSISTED_1:
                            Talk(SAY_FIRST_SQUAD_RESCUED_HORDE_0);
                            break;
                        case EVENT_FIRST_SQUAD_ASSISTED_2:
                            if (Creature* tempUnit = me->FindNearestCreature(NPC_KORKRON_INVOKER, 50.0f, true))
                                tempUnit->AI()->Talk(SAY_FIRST_SQUAD_RESCUED_HORDE_1);
                            break;
                        case EVENT_WRATH:
                            if (me->HasUnitState(UNIT_STAT_CASTING))
                                break;
                            if (me->isInCombat())
                                if (!me->getVictim()->IsFriendlyTo(me))
                                    me->CastSpell(me->getVictim(), SPELL_WRATH, false);
                            events.ScheduleEvent(EVENT_WRATH, 10000);
                            break;
                        case EVENT_HEAL:
                            if (me->isInCombat())
                            {
                                std::list<Unit*> TargetList;
                                Unit* finalTarget = me;
                                Trinity::AnyFriendlyUnitInObjectRangeCheck checker(me, me, 30.0f);
                                Trinity::UnitListSearcher<Trinity::AnyFriendlyUnitInObjectRangeCheck> searcher(me, TargetList, checker);
                                me->VisitNearbyObject(30.0f, searcher);
                                for (std::list<Unit*>::iterator itr = TargetList.begin(); itr != TargetList.end(); ++itr)
                                    if ((*itr)->GetHealthPct() < finalTarget->GetHealthPct())
                                        finalTarget = *itr;

                                uint32 spellId = SPELL_HEALING_TOUCH;
                                uint32 healthPct = finalTarget->GetHealthPct();
                                if (healthPct > 15 && healthPct < 20)
                                    spellId = (urand (0, 1) ? SPELL_REGROWTH : SPELL_HEALING_TOUCH);
                                else if (healthPct >= 20 && healthPct < 40)
                                    spellId = SPELL_REGROWTH;
                                else if (healthPct > 40)
                                    spellId = (urand(0, 1) ? SPELL_REJUVENATION : SPELL_REGROWTH);

                                me->CastSpell(finalTarget, spellId, false);
                                events.ScheduleEvent(EVENT_HEAL, 20000);
                            }
                            break;
                    }
                }
            }

            private:
                EventMap events;
                InstanceScript* instance;
        };

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_korkron_primalistAI(pCreature);
        }
};

/* ----------------------------------- Spells ----------------------------------- */

/* Overheat - 69487 */
class spell_icc_overheat : public SpellScriptLoader
{
    public:
        spell_icc_overheat() : SpellScriptLoader("spell_icc_overheat") { }

        class spell_icc_overheat_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_icc_overheat_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* passenger = GetUnitOwner()->GetVehicle()->GetPassenger(1);
                if (passenger->GetTypeId() == TYPEID_PLAYER)
                {
                    passenger->ToPlayer()->AddSpellCooldown(SPELL_CANNON_BLAST, 0, time(NULL) + 3000);
                    passenger->ToPlayer()->AddSpellCooldown(SPELL_INCINERATING_BLAST, 0, time(NULL) + 3000);
                }
            }

            void Register()
            {
                OnEffectApply += AuraEffectApplyFn(spell_icc_overheat_AuraScript::OnApply, EFFECT_0, SPELL_EFFECT_APPLY_AURA, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_icc_overheat_AuraScript();
        }
};

/* Rocket Pack - 69188 */
/* 68721 is a big red ball */
/* 69193 is the damage when landing, it does not include the visual (which is 69192) */
class spell_icc_rocket_pack : public SpellScriptLoader
{
    public:
        spell_icc_rocket_pack() : SpellScriptLoader("spell_icc_rocket_pack") { }

        class spell_icc_rocket_pack_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_icc_rocket_pack_SpellScript);

            SpellCastResult CheckRequirement()
            {
                Unit* caster = GetOriginalCaster();
                if (caster->GetTypeId() != TYPEID_PLAYER)
                    return SPELL_FAILED_TARGET_NOT_PLAYER;

                // The aura checks if the player has the aura that Zafod Boombox uses. (SPELL_EFFECT_APPLY_AREA_AURA_FRIEND)
                if (!caster->ToPlayer()->HasAura(70348)) // Rocket Pack Useable
                {
                    // May have a custom error message.
                    Spell::SendCastResult(caster->ToPlayer(), GetSpellInfo(), 0, SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW);
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
                }

                return SPELL_CAST_OK;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_icc_rocket_pack_SpellScript::CheckRequirement);
            }
        };

        class spell_icc_rocket_pack_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_icc_rocket_pack_AuraScript);

            void AfterRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                Unit* caster = GetCaster();
                caster->CastSpell(caster, 69193, true);
                // 69193 does trigger the visual AoE effect (69192) through DB
                caster->RemoveAurasDueToSpell(69188); // spell_linked_spell
                caster->RemoveAurasDueToSpell(68721); // spell_linked_spell
            }

            void Register()
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_icc_rocket_pack_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_icc_rocket_pack_SpellScript();
        }

        AuraScript* GetAuraScript() const
        {
            return new spell_icc_rocket_pack_AuraScript();
        }
};

/* Remove Rocket Pack - 70713 */
class spell_icc_remove_rocket_pack : public SpellScriptLoader
{
    public:
        spell_icc_remove_rocket_pack() : SpellScriptLoader("spell_icc_remove_rocket_pack") { }

        class spell_icc_remove_rocket_pack_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_icc_remove_rocket_pack_SpellScript);

            void HandleEffect(SpellEffIndex /*effIndex*/)
            {
                Player* hitPlr = GetHitPlayer();
                if (!hitPlr) // If player is offline
                    return;

                int32 itemId = GetEffectValue();
                uint32 itemCount = hitPlr->GetItemCount(itemId, false); // Should be 1, but just in case.
                hitPlr->DestroyItemCount(itemId, -itemCount, true, false);
            }

            void Register()
            {
                OnEffect += SpellEffectFn(spell_icc_remove_rocket_pack_SpellScript::HandleEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_icc_remove_rocket_pack_SpellScript();
        }
};

/* Rocket Artillery - 69679 */
class spell_icc_rocket_artillery_triggered : public SpellScriptLoader
{
    public:
        spell_icc_rocket_artillery_triggered() : SpellScriptLoader("spell_icc_rocket_artillery_triggered") { }

        class spell_icc_rocket_artillery_triggered_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_icc_rocket_artillery_triggered_SpellScript);

            void HandleEffect(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                //WorldLocation* targetedArea = GetTargetDest();
				WorldLocation const* const targetedArea = GetTargetDest();

                if (!targetedArea || !caster || caster->GetTypeId() != TYPEID_UNIT)
                    return;

                Position targetedPosition;
                targetedArea->GetPosition(&targetedPosition);

                std::list<mortarMarksLoc> marks = CAST_AI(npc_korkron_rocketeer::npc_korkron_rocketeerAI, caster->ToCreature()->AI())->getMarksPositions();
                for (std::list<mortarMarksLoc>::iterator itr = marks.begin(); itr != marks.end(); ++itr)
                {
                    // I love dirty code
                    Position positionInList = (*itr).location;
                    if (positionInList.m_positionX == targetedPosition.m_positionX)
                        if (positionInList.m_positionY == targetedPosition.m_positionY)
                            if (positionInList.m_positionZ == targetedPosition.m_positionZ)
                            {
                                // Don't give a fuck about orientation
                                //marks.remove(*itr);
                            }
                }
				
				CAST_AI(npc_korkron_rocketeer::npc_korkron_rocketeerAI, caster->ToCreature()->AI())->setMarks(marks);
            }

            void Register()
            {
                OnEffect += SpellEffectFn(spell_icc_rocket_artillery_triggered_SpellScript::HandleEffect, EFFECT_0, SPELL_EFFECT_TRIGGER_SPELL);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_icc_rocket_artillery_triggered_SpellScript();
        }
};

/* ---------------------------------- AreaTrigger Scripts ------------------------------------- */
class at_icc_land_frostwyrm : public AreaTriggerScript
{
    public:
        at_icc_land_frostwyrm() : AreaTriggerScript("at_icc_land_frostwyrm") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* areaTrigger)
        {
            if (InstanceScript* instance = player->GetInstanceScript())
            {
                if (instance->GetData(DATA_SPIRE_FROSTWYRM_STATE) == NOT_STARTED)
                {
                    instance->SetData(DATA_SPIRE_FROSTWYRM_STATE, IN_PROGRESS);
                    // Emote on landing seems to be 447
                    // The frostwyrm needs a script.
                    // There are 2 wyrms in fact, both land, they get in combat with the NPCs
                    // "A screeching cry pierces the air above" when she lands.
                    // The one from the opposite side does not land, however.
                }
            }

            return true;
        }
};

void AddSC_boss_gunship_battle()
{
    // Transports
    new transport_gunship();

    // Alliance
    new npc_muradin_gunship();
    new npc_the_skybreaker();
    new npc_skybreaker_rifleman();

    // Horde
    new npc_korkron_axethrower();
    new npc_korkron_primalist();

    // AreaTriggers
    new at_icc_land_frostwyrm();

    // Spells
    new spell_icc_overheat();
    new spell_icc_rocket_pack();
    new spell_icc_remove_rocket_pack();

    // Various
    new npc_zafod_boombox();
    new npc_gunship_cannon();
}
