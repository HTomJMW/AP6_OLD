[BaseContainerProps(), SCR_MapMarkerTitle()]
class SCR_AP6_MapMarkerEntryDynamic : SCR_MapMarkerEntryDynamic
{
	[Attribute("", UIWidgets.Object, "Visual configuration")]
	protected ref SCR_MarkerSimpleConfig m_EntryConfig;
	
	protected SCR_MapMarkerEntity m_markerEnt;
	
	protected ResourceName m_ussr_imageset = "{7CD99D22C7AE8195}UI/Textures/GroupManagement/FlagIcons/GroupFlagsOpfor.imageset";
	protected ResourceName m_us_imageset = "{1228936E5DB30403}UI/Textures/GroupManagement/FlagIcons/GroupFlagsBlufor.imageset";
	
	//------------------------------------------------------------------------------------------------
	void OnPlayerDeleted(int playerId, IEntity player)
	{		
		SCR_MapMarkerEntity markerEnt = m_MarkerMgr.GetDynamicMarkerByTarget(GetMarkerType(), player);
		if (markerEnt) m_MarkerMgr.RemoveDynamicMarker(markerEnt);
	}
	
	//------------------------------------------------------------------------------------------------
	void OnPlayerSpawned(int playerId, IEntity player)
	{
		if (m_MarkerMgr.GetDynamicMarkerByTarget(GetMarkerType(), player)) return;

		m_markerEnt = m_MarkerMgr.InsertDynamicMarker(GetMarkerType(), player, -1);
		m_markerEnt.SetGlobalVisible(true);

		FactionAffiliationComponent faffComp = FactionAffiliationComponent.Cast(player.FindComponent(FactionAffiliationComponent));
		
		ResourceName resName;
		switch (faffComp.GetDefaultAffiliatedFaction().GetFactionKey())
		{
			case "RUSSIA": resName = m_ussr_imageset; break;
			case "NATO": resName = m_us_imageset; break;
		}
	
		string iconQuad = "infantry";

		m_markerEnt.SetImage(resName, iconQuad);
		m_markerEnt.SetFaction(faffComp.GetDefaultAffiliatedFaction());
		m_markerEnt.SetTarget(player);
		
		EventHandlerManagerComponent ehManager = EventHandlerManagerComponent.Cast(player.FindComponent(EventHandlerManagerComponent));
		if (ehManager) ehManager.RegisterScriptHandler("OnDestroyed", this, OnDestroyed, true, true);
	}

	//------------------------------------------------------------------------------------------------
	protected void OnDestroyed(IEntity ent)
	{
		ent = SCR_EntityHelper.GetMainParent(ent, true);

		if (ent)
		{
			EventHandlerManagerComponent ehManager = EventHandlerManagerComponent.Cast(ent.FindComponent(EventHandlerManagerComponent));
			if (ehManager) ehManager.RemoveScriptHandler("OnDestroyed", this, OnDestroyed);

			SCR_MapMarkerEntity markerEnt = m_MarkerMgr.GetDynamicMarkerByTarget(GetMarkerType(), ent);
			if (markerEnt) m_MarkerMgr.RemoveDynamicMarker(markerEnt);
		}
	}

	//------------------------------------------------------------------------------------------------
	override SCR_EMapMarkerType GetMarkerType()
	{
	 	return SCR_EMapMarkerType.DYNAMIC_EXAMPLE;
	}

	//------------------------------------------------------------------------------------------------
	override void InitClientSettingsDynamic(notnull SCR_MapMarkerEntity marker, notnull SCR_MapMarkerDynamicWComponent widgetComp)
	{
		super.InitClientSettingsDynamic(marker, widgetComp);
		
		ResourceName imgset;
		Color color = Color.Black;
			
		int playerId = GetGame().GetPlayerController().GetPlayerId();
		Faction faction = SCR_FactionManager.SGetPlayerFaction(playerId);
	
		switch (faction.GetFactionKey())
		{
			case "RUSSIA": imgset = m_ussr_imageset; break;
			case "NATO": imgset = m_us_imageset; break;
		}
			
		color = faction.GetFactionColor();
	
		string iconQuad = "infantry";
	
		widgetComp.SetImage(imgset, iconQuad);
		widgetComp.SetColor(color);
		
		Widget root = widgetComp.GetRootWidget();
		ImageWidget iconWidget = ImageWidget.Cast(root.FindAnyWidget("MarkerIcon"));
		vector size = iconWidget.GetSize();
		iconWidget.SetSize(size[0] / 2, size[1] / 2);
	
		string text = "";
	
		widgetComp.SetText(text);
	}

	//------------------------------------------------------------------------------------------------
	override void InitServerLogic()
	{	
		super.InitServerLogic();

		SCR_AP6_GameMode gameMode = SCR_AP6_GameMode.GetInstance();
		if (!gameMode) return;
				
		gameMode.GetOnPlayerSpawned().Insert(OnPlayerSpawned);
		gameMode.GetOnPlayerDeleted().Insert(OnPlayerDeleted);
	}
}