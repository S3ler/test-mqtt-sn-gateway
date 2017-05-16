// TODO implement me
// test fixture: setup + connect a mqtt sn client and a mqtt client // see functional subscribtion
// connect, subscribe (normal), disconnect basically
// disconnect message of content:
// receeive no message
// without duration:
// connect+subscribe -> disconnect (valid) -> disconnect -> publish(qos0) -> disconnect
// connect+subscribe -> disconnect (invalid too short) -> disconnect -> publish(qos0) -> disconnect
// connect+subscribe -> disconnect (invalid too long) -> disconnect -> publish(qos0) -> disconnect
// overlapping with connection tests (not implemented yet)
// connect+subscribe -> disconnect (valid) -> (mqtt publish) -> connect again (clean) -> NO message -> (mqtt publish) -> NO message
// connect+subscribe -> disconnect (valid) -> (mqtt publish) -> connect again (no clean) -> NO message -> (mqtt publish) -> message
// theoratically for each disconncet message, we receive only disconnects (except after connection)

