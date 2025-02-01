[EntityEditorProps(category: "GameScripted/AP6", description: "Take Flag User Action")]
class SCR_AP6_TakeFlagAction : ScriptedUserAction
{
	protected SCR_AP6_Flag m_flag;
	protected RplComponent m_rplComponent;

	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent) 
	{
		m_flag = SCR_AP6_Flag.Cast(pOwnerEntity);
		m_rplComponent = RplComponent.Cast(pOwnerEntity.FindComponent(RplComponent));
	}

	//------------------------------------------------------------------------------------------------
	// Only on Server
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{
		if (m_rplComponent && m_rplComponent.IsProxy()) return;

		SCR_AP6_Flag flag = SCR_AP6_Flag.Cast(pOwnerEntity);

		FactionAffiliationComponent userAffiliationComponent = FactionAffiliationComponent.Cast(pUserEntity.FindComponent(FactionAffiliationComponent));
		flag.captureFlagServer(userAffiliationComponent.GetAffiliatedFactionKey());
	}

	//------------------------------------------------------------------------------------------------
	override bool HasLocalEffectOnlyScript()
	{
		return false;
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		FactionAffiliationComponent userFaffComp = FactionAffiliationComponent.Cast(user.FindComponent(FactionAffiliationComponent));
		
		SCR_AP6_GameMode gameMode = SCR_AP6_GameMode.GetInstance();
		array<SCR_AP6_Flag> flags = gameMode.getAP6Flags();

		int nextFlagId = m_flag.getNextFlagId();
		int prevFlagId = m_flag.getPreviousFlagId();

		SCR_AP6_Flag next;
		SCR_AP6_Flag prev;
		
		foreach (SCR_AP6_Flag flag : flags)
		{
			if (flag.getFlagId() == nextFlagId) next = flag;
			if (flag.getFlagId() == prevFlagId) prev = flag;
		}

		if ((next && next.getFactionKey() != userFaffComp.GetAffiliatedFactionKey()) && (prev && prev.getFactionKey() != userFaffComp.GetAffiliatedFactionKey()))
		{
			SetCannotPerformReason("Previous Flag need to be captured!");
			return false;
		}

		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		if (SCR_EditorManagerEntity.IsOpenedInstance()) return false;
		
		FactionAffiliationComponent userFaffComp = FactionAffiliationComponent.Cast(user.FindComponent(FactionAffiliationComponent));
		if (userFaffComp.GetAffiliatedFactionKey() == m_flag.getFactionKey()) return false;
		
		if (!m_flag.isAvailable()) return false;

		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{			
		outName = "[Hold] Take Flag";
		
		return true;
	}

	//------------------------------------------------------------------------------------------------
	void SCR_AP6_TakeFlagAction()
	{
	}

	//------------------------------------------------------------------------------------------------
	void ~SCR_AP6_TakeFlagAction()
	{
	}
};