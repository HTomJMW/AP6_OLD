[ComponentEditorProps(category: "GameScripted/AP6", description: "Update Victory")]
class SCR_AP6_VictoryComponentClass : ScriptComponentClass
{
};

class SCR_AP6_VictoryComponent : ScriptComponent
{
	protected SCR_AP6_GameMode m_gameMode;
	protected RplComponent m_rplComp;
	
	//------------------------------------------------------------------------------------------------
	void update()
	{
		int nato = 0;
		int russia = 0;
		
		array<SCR_AP6_Flag> flags = m_gameMode.getAP6Flags();
		
		foreach (SCR_AP6_Flag flag : flags)
		{
			if (flag.getFactionKey() == "NATO") nato++;
			if (flag.getFactionKey() == "RUSSIA") russia++;	
		}
		
		FactionManager fm = GetGame().GetFactionManager();
		
		if (nato >= flags.Count() - 1) m_gameMode.EndGameMode(SCR_GameModeEndData.CreateSimple(EGameOverTypes.ENDREASON_SCORELIMIT, -1, fm.GetFactionIndex(fm.GetFactionByKey("NATO"))));
		if (russia >= flags.Count() - 1) m_gameMode.EndGameMode(SCR_GameModeEndData.CreateSimple(EGameOverTypes.ENDREASON_SCORELIMIT, -1, fm.GetFactionIndex(fm.GetFactionByKey("RUSSIA"))));
	}

	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.INIT);
		
		m_rplComp = RplComponent.Cast(m_gameMode.FindComponent(RplComponent));
	}

	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		if (!m_rplComp.IsProxy())
		{
			GetGame().GetCallqueue().CallLater(update, 30 * 1000, true);
		}
	}

	//------------------------------------------------------------------------------------------------
	void SCR_AP6_VictoryComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
		m_gameMode = SCR_AP6_GameMode.Cast(ent);
	}

	//------------------------------------------------------------------------------------------------
	void ~SCR_AP6_VictoryComponent()
	{
	}	
};