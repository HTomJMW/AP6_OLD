class SCR_AP6_FactionRestrictionZoneClass : SCR_BaseFactionTriggerEntityClass
{
};

class SCR_AP6_FactionRestrictionZone : SCR_BaseFactionTriggerEntity
{
	protected SCR_RestrictionZoneWarningHUDComponent m_WarningHUD;
	protected vector m_pos;
	protected bool m_active = false;
	
	//------------------------------------------------------------------------------------------------
	override protected void EOnInit(IEntity owner)
	{		
		super.EOnInit(owner);
		
		m_pos = owner.GetOrigin();
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnActivate(IEntity ent)
	{
		super.OnActivate(ent);

		SCR_PlayerController pc = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		if (!pc) return;
		
		SCR_HUDManagerComponent hudManager = SCR_HUDManagerComponent.Cast(pc.FindComponent(SCR_HUDManagerComponent));
		if (hudManager)
		{
			array<BaseInfoDisplay> infoDisplays = new array<BaseInfoDisplay>;
			int count = hudManager.GetInfoDisplays(infoDisplays);
		
			for(int i = 0; i < count; i++)
       		{
	            if (infoDisplays[i].Type() == SCR_RestrictionZoneWarningHUDComponent)
				{
					m_WarningHUD = SCR_RestrictionZoneWarningHUDComponent.Cast(infoDisplays[i]);
					if (m_WarningHUD) break;
				}
       	 	}
		}

		if (m_WarningHUD && !m_active)
		{
			m_WarningHUD.ShowZoneWarning(true, ERestrictionZoneWarningType.GAMEMASTER, m_pos, GetSphereRadius() - 1, GetSphereRadius(), 3);
			m_active = true;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override protected event void OnDeactivate(IEntity ent)
	{
		super.OnDeactivate(ent);
		
		if (m_WarningHUD)
		{
			m_WarningHUD.ShowZoneWarning(false, ERestrictionZoneWarningType.GAMEMASTER, m_pos, GetSphereRadius() - 1, GetSphereRadius(), 3);
			m_active = false;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void SCR_AP6_FactionRestrictionZone(IEntitySource src, IEntity parent)
	{
		SetEventMask(EntityEvent.INIT);
	}
	
	//------------------------------------------------------------------------------------------------
	void ~SCR_AP6_FactionRestrictionZone()
	{
	}
};
