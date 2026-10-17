// ======================================================================================
// Mod Name: Radioport Fixes
// Author: Spuddeh
// Description: The station selection UI shows the Radioport's new track when the song changes.
//
//              The popup updates its track name only from VehicleRadioSongChanged, which the
//              engine sends from a car's radio controller after comparing the track name each
//              tick. The Radioport has no such controller, so while the popup is open on foot the
//              same comparison runs here, every half second, and stops when the popup closes.
// Mod Version: 1.2.0
// ======================================================================================

module RadioportDropoutFix

public class RadioportTrackNameCallback extends DelayCallback {
  public let popup: wref<VehicleRadioPopupGameController>;

  public func Call() -> Void {
    if IsDefined(this.popup) {
      this.popup.RadioportCheckTrackName();
    }
  }
}

@addField(VehicleRadioPopupGameController)
private let m_radioportTrackName: CName;

@addField(VehicleRadioPopupGameController)
private let m_radioportTrackDelay: DelayID;

@addField(VehicleRadioPopupGameController)
private let m_radioportTrackWatching: Bool;

@wrapMethod(VehicleRadioPopupGameController)
protected cb func OnPlayerAttach(playerPuppet: ref<GameObject>) -> Bool {
  let result: Bool = wrappedMethod(playerPuppet);
  if !IsDefined(this.m_playerVehicle) && IsDefined(this.m_player) {
    this.m_radioportTrackName = this.GetRadioReceiverTrackName();
    this.m_radioportTrackWatching = true;
    this.RadioportScheduleTrackCheck();
  }
  return result;
}

@wrapMethod(VehicleRadioPopupGameController)
protected func OnClose() -> Void {
  this.RadioportStopTrackCheck();
  wrappedMethod();
}

@addMethod(VehicleRadioPopupGameController)
public func RadioportCheckTrackName() -> Void {
  if !this.m_radioportTrackWatching || !IsDefined(this.m_player) {
    return;
  }
  let track: CName = this.GetRadioReceiverTrackName();
  if NotEquals(track, this.m_radioportTrackName) {
    this.m_radioportTrackName = track;
    // A switched-off Radioport keeps the name hidden, as Activate leaves it.
    if this.IsRadioReceiverActive() {
      this.SetTrackName(track);
    }
  }
  this.RadioportScheduleTrackCheck();
}

@addMethod(VehicleRadioPopupGameController)
private func RadioportScheduleTrackCheck() -> Void {
  let callback = new RadioportTrackNameCallback();
  callback.popup = this;
  // The popup slows the game; the check keeps real time.
  this.m_radioportTrackDelay = GameInstance.GetDelaySystem(this.m_player.GetGame()).DelayCallback(callback, 0.5, false);
}

@addMethod(VehicleRadioPopupGameController)
private func RadioportStopTrackCheck() -> Void {
  if this.m_radioportTrackWatching {
    this.m_radioportTrackWatching = false;
    GameInstance.GetDelaySystem(this.m_player.GetGame()).CancelCallback(this.m_radioportTrackDelay);
  }
}
