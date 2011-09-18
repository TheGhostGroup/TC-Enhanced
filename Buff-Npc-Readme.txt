Patch Created by LordPsyan.
http://www.trinitycore.net
http://www.flaminglegion.net
Patch: Buff NPC v1.1

I created a buff npc. The buff npc allows you to set spell number, name and cost in a database, split into 4 catagories.

Small - Smaller buffs. Usually set for a lower price.
Great - larger more powerful more expensive spells.
Platinum - GM 1 or higher. Good if you have a platinum membership type thing.
Tools - Good for putting conjured things like soulshards, mana stones etc.

The database (`npc_buff_spells`) is setup into 4 fields.
spell_id - Spell entry number
cat_number - Kinda the catagory numbers. You are limited to 999 spells per catagory, but more than 15 per catagory (until I fix this) might cause an error. 4 digits starting with 1 2 3 or 4. 1001-1999 is small, 2001-2999 great, 3001-3999 platinum, 4001-4999 tools.
	Example: you want to add a new spell to platinum members. entry number 12345 so cat_number would be 3010 (next available number in platinum).
cost - amount in copper the spell costs.
name - The spell name. I usually put (10 gold) with the spell name, so people know how much it costs. I left that out of the sql, and it only has the name.

The patch includes sql in "sql/All" folder.



Changelog:
v1.1

Moved spells to DB instead of hard coded.



ToDo: 
1. Make "next" and "back" buttons to allow for multiple pages of spells. This will allow for more than 15 spells per catagory.
2. Move catagories to the DB allowing more catagories.