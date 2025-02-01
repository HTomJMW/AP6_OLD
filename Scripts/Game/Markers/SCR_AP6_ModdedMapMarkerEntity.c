[EntityEditorProps(category: "GameScripted/Markers")]
modded class SCR_MapMarkerEntityClass : GenericEntityClass
{
};

// Modded SCR_MapMarkerEntity for CTF (RplProp() Target and Faction for client use)
modded class SCR_MapMarkerEntity : GenericEntity
{	
	[RplProp()]
	protected string m_iconQuad = "";
	[RplProp()]
	protected int m_MarkerFaction_index = 0;
	[RplProp()]
	protected RplId m_targetRplId = RplId.Invalid();
	
	//------------------------------------------------------------------------------------------------
	string GetQuadName()
	{
		return m_iconQuad;
	}
	
	//------------------------------------------------------------------------------------------------
	override void SetImage(string imageset, string icon)
	{
		m_sImageset = imageset;
		m_sIconName = icon;
		
		m_iconQuad = icon;
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	override Faction GetFaction()
	{
		return GetGame().GetFactionManager().GetFactionByIndex(m_MarkerFaction_index);
	}
	
	//------------------------------------------------------------------------------------------------
	override void SetFaction(Faction faction)
	{
		SCR_MapMarkerManagerComponent markerMgr = SCR_MapMarkerManagerComponent.GetInstance();
		if (!markerMgr) return;
		
		m_MarkerFaction = faction;
		
		m_MarkerFaction_index = GetGame().GetFactionManager().GetFactionIndex(faction);
		Replication.BumpMe();
		
		markerMgr.SetMarkerStreamRules(this);
	}
	
	//------------------------------------------------------------------------------------------------
	RplId GetTargetRplId()
	{
		return m_targetRplId;
	}
	
	//------------------------------------------------------------------------------------------------
	override void SetTarget(IEntity target)
	{
		m_Target = target;

		RplId rplid = RplId.Invalid();
		RplComponent rplComp = RplComponent.Cast(target.FindComponent(RplComponent));
		if (rplComp) rplid = rplComp.Id();
		
		m_targetRplId = rplid;
		Replication.BumpMe();
		
		if (target)
			SetEventMask(EntityEvent.FRAME);
		else 
			ClearEventMask(EntityEvent.FRAME);
	}
};