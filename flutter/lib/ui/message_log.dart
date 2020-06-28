import 'dart:async';
import 'dart:math';

import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'package:rocket/logic/connection_manager.dart';

class MessageLog extends StatefulWidget {
  @override
  _MessageLogState createState() => _MessageLogState();
}

class _MessageLogState extends State<MessageLog> with TickerProviderStateMixin {
  final List<String> _messages = [];

  bool isOpen;
  AnimationController controller;

  StreamSubscription<String> _listener;

  @override
  void initState() {
    super.initState();
    controller = AnimationController(
        duration: const Duration(milliseconds: 100), vsync: this)
      ..addListener(() => setState(() {}));
    isOpen = false;
  }

  @override
  Future<void> didChangeDependencies() async {
    super.didChangeDependencies();
    if (_listener != null) await _listener.cancel();

    _listener = context
        .read<ConnectionManager>()
        .logStream
        .listen((message) => setState(() => _messages.add(message)));
  }

  @override
  void dispose() {
    super.dispose();
    _listener.cancel();
  }

  @override
  Widget build(BuildContext context) {
    return Card(
      child: Column(
        children: [
          ListTile(
            leading: Transform.rotate(
              angle: controller.value * pi / 2,
              child: const Icon(Icons.arrow_right),
            ),
            onTap: () => setState(() {
              isOpen = !isOpen;
              controller.animateTo(isOpen ? 1 : 0);
            }),
            title: const Text('Message Log'),
            subtitle: (context.watch<ConnectionManager>().address == null)
                ? null
                : Text(
                    'Connected to ${context.watch<ConnectionManager>().address}\n(${(context.watch<ConnectionManager>().lossPercent * 100).toStringAsFixed(1)}% packet loss, ${context.watch<ConnectionManager>().rssi} rssi)'),
          ),
          Container(
            height: controller.value * 250,
            // padding: EdgeInsets.all(10),
            child: Scrollbar(
              child: ListView.builder(
                reverse: true,
                padding: const EdgeInsets.symmetric(vertical: 10),
                itemCount: _messages.length,
                itemBuilder: (context, i) {
                  var index = _messages.length - 1 - i;
                  var message = _messages[index];
                  return Container(
                    key: Key(index.toString()),
                    color: index % 2 == 0
                        ? const Color(0xFF555555)
                        : const Color(0xFF777777),
                    padding:
                        const EdgeInsets.symmetric(vertical: 3, horizontal: 5),
                    child: SelectableText(message),
                  );
                },
              ),
            ),
          ),
        ],
      ),
    );
  }
}
