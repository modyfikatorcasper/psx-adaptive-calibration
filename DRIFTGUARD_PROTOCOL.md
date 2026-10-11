> Lab2.1, bcdDevice0201: Input MPSX v1 pozostaje zgodny. Dodatkowe Output/Feature64 opisuje docs/LAB_PROTOCOL.md. Deskryptor gamepada został zmieniony na X/Y/Z/Rx i16 przycisków; wcześniejszy opis gamepada poniżej dotyczy bazowej wersji v1.

# Modi Raw Diagnostic HID — protocol v1

USB product: **Modi PSX Adapter**, manufacturer **Modibox**. Target: Waveshare
RP2040-Zero, 2 MB flash. Two independent USB HID interfaces; neither emulates Sony.

## Discovery and development identifiers

Development build: VID `0x1209`, PID `0x0001`, bcdDevice `0x0100`. This is the
[pid.codes private test PID](https://pid.codes/1209/0001/), shared and **not unique**.
It may only be used for private in-house testing, not redistributed, sold or
manufactured devices. Assign an approved product VID/PID before distribution.
Build overrides: `python build.py --vid 0xYOUR --pid 0xYOUR` (replace with assigned
numeric IDs). No Sony VID/PID is used.

Interface 0: Generic Desktop Game Pad (`0x01:0x05`), endpoint `0x81` IN.
Interface 1: vendor page `0xFF00`, application usage `0x0001`, endpoint `0x82` IN.
Both interrupt endpoints: 64-byte capacity, 1 ms USB interval. Raw report is exactly
**64 bytes, reportId 0**, with **no report-ID prefix**. WebHID `event.data` already
contains these 64 bytes. Select the vendor collection, then validate length, magic,
version and field constraints. The test VID/PID alone cannot identify this adapter.
No output/feature reports exist; the tester cannot configure or calibrate the pad.

## Raw input report

All multibyte integers are unsigned little-endian. Offsets are zero-based.

| Offset | Bytes | Field | Meaning |
|---|---:|---|---|
| 0–3 | 4 | magic | ASCII `MPSX` (`4D 50 53 58`) |
| 4 | 1 | version | `1` |
| 5 | 1 | controller_family | Observed family enum below; provisional model identification |
| 6 | 1 | controller_type / psx_id | Current PSX response ID; `FF` when disconnected |
| 7 | 1 | mode | 0 digital, 1 analog, 2 pressure, 3 config, 255 unavailable |
| 8 | 1 | connection_status | 0 disconnected, 1 present |
| 9 | 1 | capability_flags | Bits below; inspect per report |
| 10 | 1 | buttons_low | Original frame[3], active-low |
| 11 | 1 | buttons_high | Original frame[4], active-low |
| 12 | 1 | RX_raw | Original frame[5], unsigned 0–255 |
| 13 | 1 | RY_raw | Original frame[6], unsigned 0–255 |
| 14 | 1 | LX_raw | Original frame[7], unsigned 0–255 |
| 15 | 1 | LY_raw | Original frame[8], unsigned 0–255 |
| 16 | 1 | pressure_right | frame[9] |
| 17 | 1 | pressure_left | frame[10] |
| 18 | 1 | pressure_up | frame[11] |
| 19 | 1 | pressure_down | frame[12] |
| 20 | 1 | pressure_triangle | frame[13] |
| 21 | 1 | pressure_circle | frame[14] |
| 22 | 1 | pressure_cross | frame[15] |
| 23 | 1 | pressure_square | frame[16] |
| 24 | 1 | pressure_L1 | frame[17] |
| 25 | 1 | pressure_R1 | frame[18] |
| 26 | 1 | pressure_L2 | frame[19] |
| 27 | 1 | pressure_R2 | frame[20] |
| 28–31 | 4 | sequence | Poll attempts, including absent/invalid frames; wraps modulo 2^32 |
| 32–35 | 4 | packet_error_count | Invalid, truncated, unsupported frames or DMA timeouts; wraps |
| 36–37 | 2 | poll_interval_us | Nominal poll-start period `4500`; configuration reduces rate |
| 38–41 | 4 | timestamp_us | RP2040 time at serialization, wraps every ~71.6 min |
| 42 | 1 | native_length | 5 digital, 9 analog/config, 21 DS2; 0 disconnected |
| 43–63 | 21 | native_frame | Original valid PSX response, including header; unused tail `FF` |

Flags: bit 0 `BUTTONS_VALID`, bit 1 `AXES_VALID`, bit 2 `PRESSURE_VALID`, bit 3
`CONFIG_MODE`, bit 4 `LATEST_POLL_VALID`; bits 5–7 reserved zero. Digital axes are
zero placeholders with AXES_VALID unset, not measurements of center. Missing
pressure is zero with PRESSURE_VALID unset. Config has no usable buttons/axes.
Missing buttons use `FF FF` with BUTTONS_VALID unset.

Family enum:

| Value | Name | Observation |
|---:|---|---|
| 0 | UNKNOWN_PSX | Absent/config before family observed |
| 1 | PS1_DIGITAL | ID 41; may also be a DualShock/DS2 in digital mode |
| 2 | PS1_DUAL_ANALOG | ID 53; Dual Analog/analog joystick protocol |
| 3 | PS1_DUALSHOCK | ID 73; may also be a DS2 without pressure enabled |
| 4 | PS2_DUALSHOCK2 | ID 79 observed; capability latched until disconnect |

These names are compatibility labels, not a hardware authenticity detector. A
configuration response retains the last observed family. Once ID 79 is observed,
the family remains DS2 during mode changes in this connection. A physical
controller replacement is detected only if invalid polling lasts ≥30 ms. Swap
with a pause if comparing physical pads. Family flags never replace per-report
axis/pressure validity.

Button bits in the 16-bit active-low PSX bitmap: 0 Select, 1 L3, 2 R3, 3 Start,
4 Up, 5 Right, 6 Down, 7 Left, 8 L2, 9 R2, 10 L1, 11 R1, 12 Triangle,
13 Circle, 14 Cross, 15 Square. `pressed = !(buttons & (1 << bit))`.

## Validation, errors and hot-plug

Valid header: frame[0] is `00` or `FF`. Normally frame[2] is `5A`; Lab2.1 also accepts `00` for normal IDs41/53/73/79 after ANALOG, as documented by PSX-SPX caution3. F3 still requires5A. Native bytes remain unchanged. IDs accepted:
41, 53, 73, 79, F3. Frame length is `3 + 2*(id & 0x0F)`. Short responses never
read past the buffer. PSX has no checksum: electrically plausible corrupted axis
bytes cannot be detected by this validation alone. Each hardware poll clocks 21
bytes; a short response's extra clocks are not interpreted as data.

On an invalid poll the last correct raw sample remains byte-identical, but
LATEST_POLL_VALID is cleared. Consumers **must not** count this as a fresh sample.
An all-FF or all-00 disconnected line does not increase the error counter.
At ≥30 ms since the last valid frame, connection is cleared, family/flags/length
reset, and generic HID immediately releases all buttons and centers missing axes.
Do not interpret disconnected placeholders as native samples. Reconnection is
automatic without MCU reset, with sequence/error counters continuing.

USB sends the newest complete snapshot when its endpoint is available; intermediate
polls can be coalesced. `((seq-prev) >>> 0) - 1` measures missing poll snapshots,
not necessarily invalid controller frames. It also includes configuration pauses
only indirectly through elapsed timestamps. Rate in the tester is received USB
reports in the last second. The steady polling target is ~222 Hz; not a guarantee
of Windows/browser delivery timing. Timeout recovery bounds stalled DMA to 10 ms.

Configuration after 300 ms settling: enter43 → query45 → analog44 (unlocked) → rumble map4D → optional pressure4F → exit43 twice. Lab2.1 skips4F for model1/Dual Analog. After a fresh valid poll, ID79 completes setup; ID53 or ID73 completes it unless model3 declares DS2. Unknown/digital-only setup stops after3 passes, DS2 after at most50. Completed setup respects subsequent physical ANALOG changes instead of forcing analog again. New connection/control generation resets policy. Motor bytes always
zero; motor supply disconnected. Extra F3 exit attempts are allowed after the cap.
USB is serviced between bit-banged configuration bytes. No calibration command,
deadzone, filtering, smoothing, rescaling or forced centering enters RAW.

## Generic gamepad report (interface 0)

7 bytes, reportId 0: offsets 0 LX, 1 LY, 2 RX, 3 RY, 4 hat (low nibble),
5–6 12-bit active-high buttons (little-endian). Axes are unsigned 0–255, passed
unchanged when available. Hat: 0 Up, 1 Up-Right, 2 Right, 3 Down-Right, 4 Down,
5 Down-Left, 6 Left, 7 Up-Left, 8 neutral/null. Opposing directions cancel here;
RAW retains all original bits. Buttons 1–12: Cross, Circle, Square, Triangle,
L1, R1, L2, R2, Select, Start, L3, R3. Upper four bits zero. Generic HID uses
128 for unavailable axes and releases buttons for invalid/config/disconnected
samples; this does not change RAW. No Select+Start shortcut or Sony emulation.

## Minimal WebHID reader

```js
const [device] = await navigator.hid.requestDevice({
  filters: [{usagePage: 0xFF00, usage: 1}]
});
await device.open();
device.addEventListener('inputreport', ({reportId, data}) => {
  if (reportId !== 0 || data.byteLength !== 64) return;
  if ([77,80,83,88].some((v,i) => data.getUint8(i) !== v)) return;
  if (data.getUint8(4) !== 1) return;
  const flags = data.getUint8(9);
  if (!data.getUint8(8) || !(flags & 16) || !(flags & 2)) return;
  const rx = data.getUint8(12), ry = data.getUint8(13);
  const lx = data.getUint8(14), ly = data.getUint8(15);
  // Consume exactly these values; application statistics do not rewrite them.
});
```

Use Chrome/Edge on localhost or HTTPS. Permission is user-controlled; generic
gamepad usages may be protected from WebHID, hence the separate vendor interface.
The example device profile is a proposed integration schema; importing it into an
existing DriftGuard implementation still requires that application's integration.
