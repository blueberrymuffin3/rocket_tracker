import 'dart:convert';

abstract class Packet {
  final int rssi;
  Packet(this.rssi);

  static Packet decodePacket(String rawPacket) {
    var json = jsonDecode(rawPacket) as Map<String, dynamic>;
    switch (json['type'] as String) {
      case 'IMU':
        return IMUPacket.fromJSON(json);
      case 'GPS':
        return GPSPacket.fromJSON(json);
      case 'LOSS':
        return LossPacket.fromJSON(json);
      default:
        throw FormatException('Unknown packet type ${json['type']}');
    }
  }
}

class IMUPacket extends Packet {
  double w, x, y, z;
  IMUPacket({int rssi, this.w, this.x, this.y, this.z}) : super(rssi);
  factory IMUPacket.fromJSON(Map<String, dynamic> json) {
    var data = json['data'] as Map<String, dynamic>;
    return IMUPacket(
      rssi: (json['rssi'] as num).toInt(),
      w: (data['w'] as num).toDouble(),
      x: (data['x'] as num).toDouble(),
      y: (data['y'] as num).toDouble(),
      z: (data['z'] as num).toDouble(),
    );
  }

  @override
  String toString() {
    return 'IMUPacket($w, $x, $y, $z)';
  }
}

class GPSPacket extends Packet {
  final bool isValid;
  final double latitude, longitude, altitude;
  final int satalites;

  GPSPacket(
      {int rssi,
      this.isValid,
      this.latitude,
      this.longitude,
      this.altitude,
      this.satalites})
      : super(rssi);
  factory GPSPacket.fromJSON(Map<String, dynamic> json) {
    var data = json['data'] as Map<String, dynamic>;
    return GPSPacket(
      rssi: (json['rssi'] as num).toInt(),
      isValid: data['valid'] as bool,
      latitude: (data['lat'] as num).toDouble(),
      longitude: (data['lng'] as num).toDouble(),
      altitude: (data['alt'] as num).toDouble(),
      satalites: (data['sats'] as num).toInt(),
    );
  }

  @override
  String toString() {
    return 'GPSPacket(isValid: ${isValid}, w: ${latitude}, longitude: ${longitude}, altitude: ${altitude}, satalites: ${satalites})';
  }
}

class LossPacket extends Packet {
  final double percent;

  LossPacket({int rssi, this.percent}) : super(rssi);
  factory LossPacket.fromJSON(Map<String, dynamic> json) {
    var data = json['data'] as Map<String, dynamic>;
    return LossPacket(
      rssi: (json['rssi'] as num).toInt(),
      percent: (data['pct'] as num).toDouble(),
    );
  }

  @override
  String toString() {
    return 'LossPacket(percent: $percent)';
  }
}
