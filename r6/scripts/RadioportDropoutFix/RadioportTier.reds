// ======================================================================================
// Mod Name: Radioport Fixes
// Author: Spuddeh
// Description: The Radioport plays on foot at the radio tier the game gives it on loading.
//
//              The game sets veh_radio_tier to 0 on the Radioport's emitter when the player
//              attaches, and at 0 the Radioport plays clean. Leaving a vehicle leaves the tier on
//              the player at 1, where the Radioport's mixer plays it quieter and filtered until
//              the next load. Once the player is out, the tier is set back to the game's own 0,
//              at once and again two seconds later.
// File Version: 1.1.0
// ======================================================================================

module RadioportDropoutFix

public class RadioportTierReset extends DelayCallback {
  public func Call() -> Void {
    let player = GameInstance.GetPlayerSystem(GetGameInstance()).GetLocalPlayerMainGameObject() as PlayerPuppet;
    if !IsDefined(player) || IsDefined(player.GetMountedVehicle()) { return; }
    GameObject.AudioParameter(player, n"veh_radio_tier", 0.00, n"pocket_radio_emitter");
  }
}

// Every seat, not only the driver's: the game's own pocket radio hears only a driver leaving.
@wrapMethod(PlayerPuppet)
protected cb func OnUnmountingEvent(evt: ref<UnmountingEvent>) -> Bool {
  let result: Bool = wrappedMethod(evt);
  let child = GameInstance.FindEntityByID(this.GetGame(), evt.request.lowLevelMountingInfo.childId) as GameObject;
  if IsDefined(child) && child.IsPlayer() {
    GameObject.AudioParameter(this, n"veh_radio_tier", 0.00, n"pocket_radio_emitter");
    // Real time, so a slowed or paused game cannot hold it back.
    GameInstance.GetDelaySystem(this.GetGame()).DelayCallback(new RadioportTierReset(), 2.0, false);
  }
  return result;
}
