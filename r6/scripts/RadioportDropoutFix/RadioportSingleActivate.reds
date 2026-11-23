// ======================================================================================
// Mod Name: Radioport Fixes
// Author: Spuddeh
// Description: Choosing a station in the station selection UI applies it once.
//
//              The popup base class calls Activate on the proceed press and again on its
//              release. A release that follows a press which already activated is dropped;
//              a release with no activating press (the press arrived during the fade-in, or
//              before the popup listened) still activates, as the game does.
// Mod Version: 1.3.0
// ======================================================================================

module RadioportDropoutFix

@addField(BaseModalListPopupGameController)
private let m_radioportProceedActivated: Bool;

@wrapMethod(BaseModalListPopupGameController)
protected cb func OnAction(action: ListenerAction, consumer: ListenerActionConsumer) -> Bool {
  let isProceed: Bool = Equals(ListenerAction.GetName(action), n"proceed");
  let actionType: gameinputActionType = ListenerAction.GetType(action);
  if isProceed && Equals(actionType, gameinputActionType.BUTTON_RELEASED) && this.m_radioportProceedActivated {
    this.m_radioportProceedActivated = false;
    return false;
  }
  // The base method ignores input while the fade animation runs.
  let animating: Bool = IsDefined(this.m_inoutTransitionAnimProxy) && this.m_inoutTransitionAnimProxy.IsValid();
  let result: Bool = wrappedMethod(action, consumer);
  if isProceed && Equals(actionType, gameinputActionType.BUTTON_PRESSED) {
    this.m_radioportProceedActivated = !animating;
  }
  return result;
}
