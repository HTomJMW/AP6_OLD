[EntityEditorProps(category: "GameScripted/AP6", description: "AP6 GameMode")]
class SCR_AP6_GameModeClass : SCR_BaseGameModeClass
{
};

class SCR_AP6_GameMode : SCR_BaseGameMode
{
	[Attribute(defvalue: "true", uiwidget: UIWidgets.CheckBox, desc: "Enables defender AIs near flags.", category: "AP6 GameMode")]
	protected bool m_EnableAIs;
	
	protected ref array<SCR_AP6_Flag> m_flags = {};
	
	protected ref map<int, int> m_playersDeathTimes = new map<int, int>();
	
	//------------------------------------------------------------------------------------------------
	protected override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
	}

	//------------------------------------------------------------------------------------------------
	static SCR_AP6_GameMode GetInstance()
	{
		return SCR_AP6_GameMode.Cast(GetGame().GetGameMode());
	}
	
	//------------------------------------------------------------------------------------------------
	void enableAIs(bool enable)
	{
		m_EnableAIs = enable;
	}
	
	//------------------------------------------------------------------------------------------------
	bool isAIsEnabled()
	{
		return m_EnableAIs;
	}

	//------------------------------------------------------------------------------------------------
	protected override void OnGameStart()
	{
		super.OnGameStart();

		m_flags.Insert(SCR_AP6_Flag.Cast(GetGame().GetWorld().FindEntityByName("Flag_1")));
		m_flags.Insert(SCR_AP6_Flag.Cast(GetGame().GetWorld().FindEntityByName("Flag_2")));
		m_flags.Insert(SCR_AP6_Flag.Cast(GetGame().GetWorld().FindEntityByName("Flag_3")));
		m_flags.Insert(SCR_AP6_Flag.Cast(GetGame().GetWorld().FindEntityByName("Flag_4")));
		m_flags.Insert(SCR_AP6_Flag.Cast(GetGame().GetWorld().FindEntityByName("Flag_5")));
		
		PrintFormat("AP6 :: Flags array: %1", m_flags);
		
		if (!m_RplComponent.IsProxy())
		{
			StartGameMode();
			PrintFormat("AP6 :: GameMode running: %1", IsRunning().ToString());
			Print("AP6 :: Mission version: 0.7.10");

			if (RplSession.Mode() == RplMode.Dedicated)
			{
				Print("AP6 :: Server: Dedicated");
			} else {
				Print("AP6 :: Server: Player-Hosted");
			}
		}
		
		// For discord BOT
		Print("Mission: AP6 v0.7.10");
		
		BaseWorld world = GetGame().GetWorld();
		if (!world) return;
		world.QueryEntitiesBySphere(world.FindEntityByName("Flag_5").GetOrigin(), 35, RemoveEntities, FilterEntities, EQueryEntitiesFlags.STATIC);
		world.UpdateEntities();
	}
	
	//------------------------------------------------------------------------------------------------
	bool RemoveEntities(IEntity ent)
	{
		ent.SetOrigin("0 0 0");
	
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	bool FilterEntities(IEntity ent)
	{
		if (!ent) return false;
	
		if (ent.Type() == (typename)Tree || ent.GetPrefabData().GetPrefabName() == "{926C09375D2DAA4D}PrefabLibrary/Rocks/Granite/GraniteStone_01.et" || ent.GetPrefabData().GetPrefabName() == "{1BA4712EED0CB208}PrefabLibrary/Rocks/Granite/GraniteStone_03.et") return true;
		
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	array<SCR_AP6_Flag> getAP6Flags()
	{
		return m_flags;
	}

	//------------------------------------------------------------------------------------------------
	override void OnPlayerDisconnected(int playerId, KickCauseCode cause, int timeout)
	{
		super.OnPlayerDisconnected(playerId, cause, timeout);
		
		m_playersDeathTimes.Remove(playerId);
	}

	//------------------------------------------------------------------------------------------------
	override void OnPlayerRegistered(int playerId)
	{
		super.OnPlayerRegistered(playerId);
	}

	//------------------------------------------------------------------------------------------------
	override void OnPlayerAuditSuccess(int iPlayerID)
	{
		super.OnPlayerAuditSuccess(iPlayerID);
	}

	//------------------------------------------------------------------------------------------------
	override void OnPlayerSpawnFinalize_S(SCR_SpawnRequestComponent requestComponent, SCR_SpawnHandlerComponent handlerComponent, SCR_SpawnData data, IEntity entity)
	{
		super.OnPlayerSpawnFinalize_S(requestComponent, handlerComponent, data, entity);
	}

	//------------------------------------------------------------------------------------------------
	override void OnPlayerKilled(int playerId, IEntity playerEntity, IEntity killerEntity, notnull Instigator killer)
	{
		super.OnPlayerKilled(playerId, playerEntity, killerEntity, killer);
		
		int lastDeath;
		m_playersDeathTimes.Find(playerId, lastDeath);
		int currentMissionTime = GetElapsedTime();
		m_playersDeathTimes.Set(playerId, currentMissionTime);

		SCR_RespawnTimerComponent rtc = SCR_RespawnTimerComponent.Cast(this.FindComponent(SCR_RespawnTimerComponent));
		if (rtc)
		{
			switch (true)
			{
				case (currentMissionTime - lastDeath < 15): rtc.SetRespawnTime(playerId, 30); break;
				case (currentMissionTime - lastDeath < 30): rtc.SetRespawnTime(playerId, 20); break;
				default: rtc.SetRespawnTime(playerId, 15); break;
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	void SCR_AP6_GameMode(IEntitySource src, IEntity parent)
	{
	}

	//------------------------------------------------------------------------------------------------
	void ~SCR_AP6_GameMode()
	{
		if (m_flags) m_flags.Clear();
		m_flags = null;

		if (m_playersDeathTimes) m_playersDeathTimes.Clear();
		m_playersDeathTimes = null;
	}
};