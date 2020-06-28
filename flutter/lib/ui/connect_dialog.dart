import 'dart:io';

import 'package:flutter/material.dart';

class _ConnectionDialog extends StatefulWidget {
  @override
  _ConnectionDialogState createState() => _ConnectionDialogState();
}

class _ConnectionDialogState extends State<_ConnectionDialog> {
  static const optionPadding =
      EdgeInsets.symmetric(vertical: 8.0, horizontal: 24.0);
  static final _addressRegExp = RegExp(r'^[A-Za-z0-9\.]+:[0-9]+$');
  static final _addressFieldcontroller =
      TextEditingController(text: '192.168.1.179:8266')
        ..selection = const TextSelection(baseOffset: 10, extentOffset: 13);
  static final _addressFieldKey = GlobalKey<FormFieldState<String>>();

  bool _isConnecting = false;
  String _message;

  Future<void> _onSubmitted(BuildContext context) async {
    if (!_addressFieldKey.currentState.validate()) {
      setState(() {
        _message = null;
      });
      return;
    }

    setState(() {
      _message = 'Connecting...';
      _isConnecting = true;
    });

    var address = _addressFieldKey.currentState.value.split(':');
    assert(address.length == 2);
    var host = address[0];
    var port = int.parse(address[1]);

    print('Connecting to ${host}:${port}...');

    Socket _socket;

    try {
      _socket =
          await Socket.connect(host, port, timeout: const Duration(seconds: 5));
    } on SocketException catch (e) {
      print('Error connecting: ${e}');
      setState(() {
        _message = e.toString();
        _isConnecting = false;
      });
      return;
    }
    print('Connected successfully');
    setState(() {
      _isConnecting = false;
    });
    Navigator.pop(context, _socket);
  }

  @override
  Widget build(BuildContext context) {
    var addressFormField = TextFormField(
      key: _addressFieldKey,
      controller: _addressFieldcontroller,
      autofocus: true,
      decoration: const InputDecoration(
        border: OutlineInputBorder(),
        labelText: 'Address',
      ),
      onFieldSubmitted: _isConnecting ? null : (_) => _onSubmitted(context),
      validator: (String value) {
        if (_addressRegExp.hasMatch(value)) {
          return null;
        } else {
          return 'Invalid Address (host:port)';
        }
      },
    );

    var submitButton = OutlineButton(
      child: const Text('Connect'),
      onPressed: _isConnecting ? null : () => _onSubmitted(context),
    );

    return WillPopScope(
      onWillPop: () async => false, // Disable back button
      child: SimpleDialog(
        title: const Text('Connect to rocket'),
        children: [
          SizedBox(
            width: 500,
            child: Column(children: [
              Padding(padding: optionPadding, child: Text(_message ?? '')),
              Padding(padding: optionPadding, child: addressFormField),
              Padding(padding: optionPadding, child: submitButton),
            ]),
          )
        ],
      ),
    );
  }
}

Future<Socket> showConnectionDialog(BuildContext context) {
  return showDialog<Socket>(
    context: context,
    barrierDismissible: false,
    builder: (_) => _ConnectionDialog(),
  );
}
