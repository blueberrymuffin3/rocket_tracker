import 'dart:async';
import 'dart:convert';
import 'dart:io';

import 'package:flutter/material.dart';
import 'package:pedantic/pedantic.dart';
import 'package:rocket/logic/packet_decoder.dart';
import 'package:rocket/ui/connect_dialog.dart';

class ConnectionManager extends ChangeNotifier {
  Stream<String> get rawPacketStream => _rawPacketStreamController.stream;
  Stream<Packet> get packetStream => _packetStreamController.stream;
  Stream<IMUPacket> get imuPacketStream => _imuPacketStreamController.stream;
  Stream<GPSPacket> get gpsPacketStream => _gpsPacketStreamController.stream;
  double get lossPercent => _lossPercent;
  int get rssi => _rssi;

  Stream<String> get logStream => _logStreamController.stream;

  String get address => _socket == null
      ? null
      : '${_socket.address.address}:${_socket.remotePort}';

  double _lossPercent = 0;
  int _rssi = 0;

  final StreamController<String> _rawPacketStreamController =
      StreamController.broadcast();
  final StreamController<Packet> _packetStreamController =
      StreamController.broadcast();
  final StreamController<IMUPacket> _imuPacketStreamController =
      StreamController.broadcast();
  final StreamController<GPSPacket> _gpsPacketStreamController =
      StreamController.broadcast();

  final StreamController<String> _logStreamController =
      StreamController.broadcast();

  Socket _socket;
  ConnectionManager() {
    rawPacketStream.listen((rawPacket) {
      try {
        var packet = Packet.decodePacket(rawPacket);
        _logStreamController.add(packet.toString());
        _packetStreamController.add(packet);
      } catch (e) {
        _logStreamController.add(e.toString());
      }
    });

    packetStream.listen((packet) {
      _rssi = packet.rssi;
    });

    packetStream
        .where((packet) => packet is IMUPacket)
        .map((packet) => packet as IMUPacket)
        .pipe(_imuPacketStreamController.sink);
    packetStream
        .where((packet) => packet is GPSPacket)
        .map((packet) => packet as GPSPacket)
        .pipe(_gpsPacketStreamController.sink);
    packetStream
        .where((packet) => packet is LossPacket)
        .map((packet) => packet as LossPacket)
        .listen((packet) {
      _lossPercent = packet.percent;
      notifyListeners();
    });
  }

  Future<void> promptConnect(BuildContext context) async {
    if (_socket != null) {
      _logStreamController.add('Disconnecting from $address');
      _socket.destroy();
      _socket = null;
      notifyListeners();
    }

    _socket = await showConnectionDialog(context);
    notifyListeners();

    unawaited(_rawPacketStreamController
        .addStream(utf8.decoder.bind(_socket).transform(const LineSplitter())));

    _logStreamController.add('Connected to $address');
  }
}
