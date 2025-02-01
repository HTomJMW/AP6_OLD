[EntityEditorProps(category: "GameScripted/AP6", description: "AP6 Flag")]
class SCR_AP6_FlagClass : GenericEntityClass
{
};

class SCR_AP6_Flag : GenericEntity
{
	[Attribute("FIA", UIWidgets.EditBox, "Owner's FactionKey (FIA, RUSSIA, NATO)"), RplProp(onRplName: "OnCapturedClient", condition: RplCondition.Custom, customConditionName: "RpcCondition")]
	protected FactionKey m_ownerFactionKey;
	
	[Attribute(defvalue: "0", params: "0 inf", desc: "Flag Id. (Need to be unique!)")]
	protected int m_flagId;
	[Attribute(defvalue: "0", params:" 0 inf", desc: "Previorus Flag Id.")]
	protected int m_previorusFlagId;
	[Attribute(defvalue: "0", params:" 0 inf", desc: "Next Flag Id.")]
	protected int m_nextFlagId;
	[Attribute( defvalue: "1", uiwidget: UIWidgets.CheckBox, desc: "Allow capture" )]
	bool m_available;
	
	protected SCR_FlagComponent m_flagComponent;
	protected SlotManagerComponent m_slotManagerComp;
	protected SCR_SpawnPoint m_spawnPoint;
	protected SCR_MapDescriptorComponent m_mapDescriptor;
	protected SCR_MapEntity m_mapEntity;
	protected float m_zoomValue;
	
	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		m_slotManagerComp = SlotManagerComponent.Cast(owner.FindComponent(SlotManagerComponent));
		EntitySlotInfo slot = m_slotManagerComp.GetSlotByName("SpawnPoint");
		m_spawnPoint = SCR_SpawnPoint.Cast(slot.GetAttachedEntity());
		m_spawnPoint.SetSpawnPointName("SpawnPoint");
		m_spawnPoint.SetFactionKey(m_ownerFactionKey);
		m_spawnPoint.SetSpawnRadius(12);
		
		m_mapDescriptor = SCR_MapDescriptorComponent.Cast(owner.FindComponent(SCR_MapDescriptorComponent));
		initMarker();
		
		m_mapEntity = SCR_MapEntity.GetMapInstance();
		m_mapEntity.GetOnMapOpen().Insert(OnPlayerMapOpen);
		m_mapEntity.GetOnMapZoom().Insert(OnPlayerMapZoom);
		m_mapEntity.GetOnMapClose().Insert(OnPlayerMapClose);
		
		m_flagComponent = SCR_FlagComponent.Cast(owner.FindComponent(SCR_FlagComponent));
		changeFlag(m_ownerFactionKey);
	}
	
	//------------------------------------------------------------------------------------------------
	bool RpcCondition()
	{
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	FactionKey getFactionKey()
	{
		return m_ownerFactionKey;
	}
	
	//------------------------------------------------------------------------------------------------
	int getFlagId()
	{
		return m_flagId;
	}
	
	//------------------------------------------------------------------------------------------------
	int getNextFlagId()
	{
		return m_nextFlagId;
	}
	
	//------------------------------------------------------------------------------------------------
	int getPreviousFlagId()
	{
		return m_previorusFlagId;
	}
	
	//------------------------------------------------------------------------------------------------
	bool isAvailable()
	{
		return m_available;
	}
	
	//------------------------------------------------------------------------------------------------
	// Server
	void captureFlagServer(FactionKey newFactionKey)
	{
		m_ownerFactionKey = newFactionKey;
		changeFlag(newFactionKey);
		m_spawnPoint.SetFactionKey(newFactionKey);
		changeMarkerColor(m_ownerFactionKey);
		
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	// Proxys
	protected void OnCapturedClient()
	{
		changeFlag(m_ownerFactionKey);
		changeMarkerColor(m_ownerFactionKey);
		playSound();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void changeFlag(FactionKey factionkey)
	{
		switch (factionkey)
  		{
  			case "NATO": m_flagComponent.ChangeMaterial("{610803CC3F3D6649}Assets/Props/Fabric/Flags/Data/Flag_1_2_USA.emat", ""); break;
  			case "RUSSIA": m_flagComponent.ChangeMaterial("{D33A1334B4A3369D}Assets/Props/Fabric/Flags/Data/Flag_1_2_USSR.emat", ""); break;
  			case "FIA": m_flagComponent.ChangeMaterial("{9BE1341F99D33483}Assets/Props/Fabric/Flags/Data/Flag_1_2_EVERON.emat", ""); break;
  		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void playSound()
	{
		AudioSystem.PlaySound("{03D87648DB8A7641}Sounds/UI/Samples/Campaign/UI_Campaign_Siren.wav");
	}
	
	//------------------------------------------------------------------------------------------------
	protected void initMarker()
	{
		if (!GetGame().InPlayMode()) return;
		
		FactionManager fm = GetGame().GetFactionManager();
		
		MapItem mapItem = m_mapDescriptor.Item(); 
	
		mapItem.SetDisplayName("");
		mapItem.SetBaseType(EMapDescriptorType.MDT_ICON);
		mapItem.SetImageDef("AP6_Marker");
		
		MapDescriptorProps props = mapItem.GetProps();
			props.SetDetail(96);
			props.SetIconSize(1.25, 1, 1);
			props.SetTextSize(16, 8, 32);
			props.SetTextColor(fm.GetFactionByKey(m_ownerFactionKey).GetFactionColor());
			props.SetFrontColor(fm.GetFactionByKey(m_ownerFactionKey).GetFactionColor());
			props.SetTextVisible(true);
			props.SetIconVisible(true);
			props.Activate(true);

		mapItem.SetProps(props);
	}

	//------------------------------------------------------------------------------------------------
	protected void changeMarkerColor(FactionKey newOwnerFactionKey)
	{
		FactionManager fm = GetGame().GetFactionManager();
		
		MapItem mapItem = m_mapDescriptor.Item();
		
		MapDescriptorProps props = mapItem.GetProps();
			props.SetFrontColor(fm.GetFactionByKey(newOwnerFactionKey).GetFactionColor());
		mapItem.SetProps(props);
	}	
	
	//------------------------------------------------------------------------------------------------
	protected void OnPlayerMapOpen(MapConfiguration config)
	{
		m_zoomValue = m_mapEntity.GetCurrentZoom();
	}

	//------------------------------------------------------------------------------------------------
	protected void OnPlayerMapZoom(float pixelPerUnit)
	{
		float currentZoom = m_mapEntity.GetCurrentZoom(); // 0.0-20.0 ZOOM

		MapItem item = m_mapDescriptor.Item();
		if (item)
		{
			MapDescriptorProps props = item.GetProps();
				props.SetIconSize(1.25, -currentZoom * 1.25, -currentZoom * 1.25);
			
			item.SetProps(props);
		}
	}

	//------------------------------------------------------------------------------------------------
	protected void OnPlayerMapClose(MapConfiguration config)
	{
	}

	//------------------------------------------------------------------------------------------------
	void SCR_AP6_Flag(IEntitySource src, IEntity parent)
	{
	}

	//------------------------------------------------------------------------------------------------
	void ~SCR_AP6_Flag()
	{
		m_mapEntity.GetOnMapOpen().Remove(OnPlayerMapOpen);
		m_mapEntity.GetOnMapZoom().Remove(OnPlayerMapZoom);
		m_mapEntity.GetOnMapClose().Remove(OnPlayerMapClose);
	}
};