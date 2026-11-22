// ======================================================================================
// Mod Name: Radioport Off
// Author: Spuddeh
// Description: Turns the Radioport off for good. Its button does nothing, holding it opens no
//              station list, and getting out of a vehicle does not turn it on. On a metro ride
//              the button and the station list still reach the train's radio.
// Mod Version: 1.0.0
// ======================================================================================

module RadioportOff

@wrapMethod(PocketRadio)
public final func HandleInputAction(action: ListenerAction) -> Void {
  if this.m_isInMetro {
    wrappedMethod(action);
  }
}

@wrapMethod(PocketRadio)
private final func TurnOn(playSFX: Bool) -> Void {}

@wrapMethod(PocketRadioWheelDecisions)
protected final const func EnterCondition(const stateContext: ref<StateContext>, const scriptInterface: ref<StateGameScriptInterface>) -> Bool {
  if !StatusEffectSystem.ObjectHasStatusEffectWithTag(scriptInterface.executionOwner, n"MetroRide") {
    return false;
  }
  return wrappedMethod(stateContext, scriptInterface);
}
