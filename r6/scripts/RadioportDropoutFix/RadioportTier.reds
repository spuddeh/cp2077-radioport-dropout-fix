// ======================================================================================
// Mod Name: Radioport Fixes
// Author: Spuddeh
// Description: The Radioport plays on foot at the radio tier the game gives it on loading.
//
//              The game sets veh_radio_tier to 0 on the Radioport's emitter when the player
//              attaches, and at 0 the Radioport plays clean. Leaving a vehicle leaves the tier on
//              the player at 1, where the Radioport's mixer plays it quieter and filtered until
//              the next load. Once the player is out, the tier is set back to the game's own 0,
//              as the unmount event arrives.
// File Version: 1.1.0
// ======================================================================================

module RadioportDropoutFix

// Every seat, not only the driver's: the game's own pocket radio hears only a driver leaving.
@wrapMethod(PlayerPuppet)
protected cb func OnUnmountingEvent(evt: ref<UnmountingEvent>) -> Bool {
  let result: Bool = wrappedMethod(evt);
  let child = GameInstance.FindEntityByID(this.GetGame(), evt.request.lowLevelMountingInfo.childId) as GameObject;
  // A seat switch arrives as a silent unmount while the player stays in the vehicle, where tier 2 must hold.
  let data = evt.request.mountData;
  let silent = IsDefined(data) && IsDefined(data.mountEventOptions) && data.mountEventOptions.silentUnmount;
  if IsDefined(child) && child.IsPlayer() && !silent {
    GameObject.AudioParameter(this, n"veh_radio_tier", 0.00, n"pocket_radio_emitter");
  }
  return result;
}
