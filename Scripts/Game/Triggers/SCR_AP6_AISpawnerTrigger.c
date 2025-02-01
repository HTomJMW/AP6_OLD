class SCR_AP6_AISpawnerTriggerClass : ScriptedGameTriggerEntityClass
{
};

class SCR_AP6_AISpawnerTrigger : ScriptedGameTriggerEntity
{
	protected SCR_AP6_GameMode m_gameMode;
	protected SCR_AP6_Flag m_parentFlag;
	protected RplComponent m_rplComponent;
	
	protected ref array<SCR_AIGroup> m_groups = {};
	protected ref array<AIWaypoint> m_wayPoints = {};
	
	protected ResourceName m_FIA_FireGroup = "{5BEA04939D148B1D}Prefabs/Groups/INDFOR/Group_FIA_FireTeam.et";
	protected ResourceName m_FIA_SniperGroup = "{6307F42403E9B8A4}Prefabs/Groups/INDFOR/Group_FIA_SharpshooterTeam.et";
	protected ResourceName m_FIA_MGGroup = "{22F33D3EC8F281AB}Prefabs/Groups/INDFOR/Group_FIA_MachineGunTeam.et";
	protected ResourceName m_FIA_ATGroup = "{2CC26054775FBA2C}Prefabs/Groups/INDFOR/Group_FIA_Team_AT.et";
	
	protected ResourceName m_RUSSIA_FireGroup = "{28574BDF21397789}Prefabs/Groups/OPFOR/RHS_AFRF/MSV/VKPO_Summer/Group_RHS_RF_MSV_VKPO_S_FireGroup.et";
	protected ResourceName m_RUSSIA_MedicalGroup = "{50844098F3ABB60F}Prefabs/Groups/OPFOR/RHS_AFRF/MSV/VKPO_Summer/Group_RHS_RF_MSV_VKPO_S_MedicalSection.et";
	protected ResourceName m_RUSSIA_MGGroup = "{2A667B5C63C8A92D}Prefabs/Groups/OPFOR/RHS_AFRF/MSV/VKPO_Summer/Group_RHS_RF_MSV_VKPO_S_MachineGunTeam.et";
	//protected ResourceName m_RUSSIA_ATGroup = "{3C3A6867982DC57A}Prefabs/Groups/OPFOR/RHS_AFRF/MSV/VKPO_Summer/Group_RHS_RF_MSV_VKPO_S_Team_AT.et";
	
	protected ResourceName m_NATO_FireGroup = "{3144936D0BFAAC43}Prefabs/Groups/BLUFOR/RHS_USAF/RHS_USAF_USMC_MEF_D/Group_USAF_USMC_MEF_D_FireTeam.et";
	protected ResourceName m_NATO_MedicalGroup = "{D405F2799352AB58}Prefabs/Groups/BLUFOR/RHS_USAF/RHS_USAF_USMC_MEF_D/Group_USAF_USMC_MEF_D_MedicalSection.et";
	protected ResourceName m_NATO_MGGroup = "{AEE7C9BD0331B47A}Prefabs/Groups/BLUFOR/RHS_USAF/RHS_USAF_USMC_MEF_D/Group_USAF_USMC_MEF_D_MachineGunTeam.et";
	//protected ResourceName m_NATO_ATGroup = "{1910D43E4DF60884}Prefabs/Groups/BLUFOR/RHS_USAF/RHS_USAF_USMC_MEF_D/Group_USAF_USMC_MEF_D_Team_LAT.et";
	
	protected ResourceName m_defWayPoint = "{93291E72AC23930F}Prefabs/AI/Waypoints/AIWaypoint_Defend.et";
	
	//------------------------------------------------------------------------------------------------
	override protected void OnInit(IEntity owner)
	{
		m_gameMode = SCR_AP6_GameMode.GetInstance();
		if (!m_gameMode.isAIsEnabled())
		{
			SetUpdateRate(300);
			return;
		}
		
		m_rplComponent = RplComponent.Cast(owner.FindComponent(RplComponent));
		m_parentFlag = SCR_AP6_Flag.Cast(owner.GetParent());

		if (GetGame().InPlayMode())
		{
			for (int i = 0; i < 5; i++)
			{
				spawnWayPoint(m_defWayPoint);
			}
			
			RandomGenerator randomgen = new RandomGenerator();
			int rnd = randomgen.RandIntInclusive(0, 2);
			
			GetGame().GetCallqueue().CallLater(spawnAIGroup, 3000, false, m_FIA_FireGroup);
			GetGame().GetCallqueue().CallLater(spawnAIGroup, 6000, false, m_FIA_MGGroup);
			
			switch (rnd)
			{
				case 0:
				{
					GetGame().GetCallqueue().CallLater(spawnAIGroup, 9000, false, m_FIA_SniperGroup);
					break;
				} 
				case 1:
				{
					GetGame().GetCallqueue().CallLater(spawnAIGroup, 9000, false, m_FIA_ATGroup);
					break;
				}
				case 2:
				{
					GetGame().GetCallqueue().CallLater(spawnAIGroup, 9000, false, m_FIA_FireGroup);
					break;
				}
			}
			
			GetGame().GetCallqueue().CallLater(manageGroupWayPoints, 30000, true);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnActivate(IEntity ent)
	{
		if (m_rplComponent.IsProxy()) return;
		if (!m_gameMode.isAIsEnabled()) return;

		array<IEntity> outEntities = {};
		GetEntitiesInside(outEntities);
		
		foreach (IEntity entity : outEntities)
		{
			DamageManagerComponent dmc = DamageManagerComponent.Cast(entity.FindComponent(DamageManagerComponent));
			if (dmc.IsDestroyed()) outEntities.RemoveItem(entity);
		}
		
		for (int i = m_groups.Count() - 1; i >= 0; i--)
		{
			if (!m_groups[i]) m_groups.Remove(i);
		}
		
		if (outEntities.IsEmpty() && m_groups.IsEmpty())
		{
			switch (m_parentFlag.getFactionKey())
			{
				case "RUSSIA": spawnAIGroup(m_RUSSIA_FireGroup); spawnAIGroup(m_RUSSIA_MedicalGroup); spawnAIGroup(m_RUSSIA_MGGroup); break;
				case "NATO": spawnAIGroup(m_NATO_FireGroup); spawnAIGroup(m_NATO_MedicalGroup); spawnAIGroup(m_NATO_MGGroup); break;
				case "FIA": spawnAIGroup(m_FIA_FireGroup); spawnAIGroup(m_FIA_SniperGroup); spawnAIGroup(m_FIA_MGGroup); break;
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void spawnAIGroup(ResourceName resName)
	{
		if (m_rplComponent.IsProxy()) return;

		EntitySpawnParams spawnParams = new EntitySpawnParams;
		spawnParams.TransformMode = ETransformMode.WORLD;

		vector mat[4];
		m_parentFlag.GetTransform(mat);
		
		RandomGenerator randomgen = new RandomGenerator();
		vector rndpos = randomgen.GenerateRandomPointInRadius(3, 12, mat[3], true);
		vector emptyPos;
		SCR_WorldTools.FindEmptyTerrainPosition(emptyPos, rndpos, 6);
		mat[3] = emptyPos;
		
		spawnParams.Transform = mat;
		
		SCR_AIGroup group = SCR_AIGroup.Cast(GetGame().SpawnEntityPrefab(Resource.Load(resName), GetGame().GetWorld(), spawnParams));
		if (group) m_groups.Insert(group);
		
		group.AddWaypoint(m_wayPoints.GetRandomElement());
	}
	
	//------------------------------------------------------------------------------------------------
	protected void spawnWayPoint(ResourceName resName)
	{
		if (m_rplComponent.IsProxy()) return;
	
		EntitySpawnParams spawnParams = new EntitySpawnParams;
		spawnParams.TransformMode = ETransformMode.WORLD;

		vector mat[4];
		m_parentFlag.GetTransform(mat);
		
		RandomGenerator randomgen = new RandomGenerator();
		vector rndpos = randomgen.GenerateRandomPointInRadius(15, 50, mat[3], true);
		vector emptyPos;
		SCR_WorldTools.FindEmptyTerrainPosition(emptyPos, rndpos, 10);
		mat[3] = emptyPos;
		
		spawnParams.Transform = mat;
	
		SCR_AIWaypoint wayPoint = SCR_AIWaypoint.Cast(GetGame().SpawnEntityPrefab(Resource.Load(resName), GetGame().GetWorld(), spawnParams));
		if (wayPoint) m_wayPoints.Insert(wayPoint);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void manageGroupWayPoints()
	{
		if (m_rplComponent.IsProxy()) return;
	
		foreach (SCR_AIGroup group : m_groups)
		{
			if (!group) continue;
			if (!group.GetCurrentWaypoint()) group.AddWaypoint(m_wayPoints.GetRandomElement());
		}
	}
};