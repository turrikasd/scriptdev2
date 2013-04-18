/* Copyright (C) 2006 - 2013 ScriptDev2 <http://www.scriptdev2.com/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* ScriptData
SDName: Boss_Blackheart_the_Inciter
SD%Complete: 80
SDComment: Incite Chaos needs further research and core support
SDCategory: Auchindoun, Shadow Labyrinth
EndScriptData */

#include "precompiled.h"
#include "shadow_labyrinth.h"

enum
{
    SPELL_INCITE_CHAOS      = 33676,                        // triggers 33684 on party members - needs core support
    SPELL_CHARGE            = 33709,
	SPELL_H_CHARGE			= 43519,
    SPELL_WAR_STOMP         = 33707,

    SAY_INTRO1              = -1555008,
    SAY_INTRO2              = -1555009,
    SAY_INTRO3              = -1555010,
    SAY_AGGRO1              = -1555011,
    SAY_AGGRO2              = -1555012,
    SAY_AGGRO3              = -1555013,
    SAY_SLAY1               = -1555014,
    SAY_SLAY2               = -1555015,
    SAY_HELP                = -1555016,
    SAY_DEATH               = -1555017,

    SAY2_INTRO1             = -1555018,
    SAY2_INTRO2             = -1555019,
    SAY2_INTRO3             = -1555020,
    SAY2_AGGRO1             = -1555021,
    SAY2_AGGRO2             = -1555022,
    SAY2_AGGRO3             = -1555023,
    SAY2_SLAY1              = -1555024,
    SAY2_SLAY2              = -1555025,
    SAY2_HELP               = -1555026,
    SAY2_DEATH              = -1555027,
};

struct MANGOS_DLL_DECL boss_blackheart_the_inciterAI : public ScriptedAI
{
    boss_blackheart_the_inciterAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
	bool m_bIsRegularMode;
	bool m_calledforHelp;

    uint32 m_uiInciteChaosTimer;
    uint32 m_uiChargeTimer;
    uint32 m_uiKnockbackTimer;
	uint32 m_helpTimer;

    void Reset() override
    {
		m_calledforHelp		 = false;
		m_helpTimer			 = 5000;
        m_uiInciteChaosTimer = 50000;
        m_uiChargeTimer      = urand(30000, 37000);
        m_uiKnockbackTimer   = urand(10000, 14000);
    }

    void KilledUnit(Unit* /*pVictim*/) override
    {
		switch (urand(0, 4))
		{
			case 0: DoScriptText(SAY_SLAY1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY2, m_creature); break;
            case 2: DoScriptText(SAY2_SLAY1, m_creature); break;
			case 3: DoScriptText(SAY2_SLAY2, m_creature); break;
		}
    }

    void JustDied(Unit* /*pKiller*/) override
    {
		DoScriptText(urand(0, 1) ? SAY_DEATH : SAY2_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_INCITER, DONE);
    }

    void Aggro(Unit* /*pWho*/) override
    {
        switch (urand(0, 5))
        {
            case 0: DoScriptText(SAY_AGGRO1, m_creature); break;
            case 1: DoScriptText(SAY_AGGRO2, m_creature); break;
            case 2: DoScriptText(SAY_AGGRO3, m_creature); break;
			case 3: DoScriptText(SAY2_AGGRO1, m_creature); break;
			case 4: DoScriptText(SAY2_AGGRO2, m_creature); break;
			case 5: DoScriptText(SAY2_AGGRO3, m_creature); break;
        }

        if (m_pInstance)
            m_pInstance->SetData(TYPE_INCITER, IN_PROGRESS);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_INCITER, FAIL);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // Return since we have no pTarget
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		if (!m_calledforHelp)
		{
			if (m_helpTimer < uiDiff)
			{
				m_creature->CallForHelp(70.0f); // Get owned if you didn't clear the room
				DoScriptText(urand(0, 1) ? SAY_HELP : SAY2_HELP, m_creature);
				m_calledforHelp = true;
			} else m_helpTimer -= uiDiff;
		}

        // ToDo: this needs future core and script support
        /*if (m_uiInciteChaosTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_INCITE_CHAOS) == CAST_OK)
                m_uiInciteChaosTimer = 40000;
        }
        else
            m_uiInciteChaosTimer -= uiDiff;*/

        // Charge Timer
        if (m_uiChargeTimer < uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            if (pTarget && DoCastSpellIfCan(pTarget, (m_bIsRegularMode ? SPELL_CHARGE : SPELL_H_CHARGE)) == CAST_OK)
                m_uiChargeTimer = urand(30000, 43000);
        }
        else
            m_uiChargeTimer -= uiDiff;

        // Knockback Timer
        if (m_uiKnockbackTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_WAR_STOMP) == CAST_OK)
                m_uiKnockbackTimer = urand(15000, 30000);
        }
        else
            m_uiKnockbackTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_blackheart_the_inciter(Creature* pCreature)
{
    return new boss_blackheart_the_inciterAI(pCreature);
}

void AddSC_boss_blackheart_the_inciter()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_blackheart_the_inciter";
    pNewScript->GetAI = &GetAI_boss_blackheart_the_inciter;
    pNewScript->RegisterSelf();
}
