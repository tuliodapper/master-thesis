# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: contract_out.proto

import sys
_b=sys.version_info[0]<3 and (lambda x:x) or (lambda x:x.encode('latin1'))
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()




DESCRIPTOR = _descriptor.FileDescriptor(
  name='contract_out.proto',
  package='contract_out',
  syntax='proto2',
  serialized_options=None,
  serialized_pb=_b('\n\x12\x63ontract_out.proto\x12\x0c\x63ontract_out\"\xbc\x06\n\x0b\x43ontractOut\x12\r\n\x05isACK\x18\x01 \x02(\x08\x12\x38\n\nupdateUser\x18\x02 \x03(\x0b\x32$.contract_out.ContractOut.UpdateUser\x12H\n\x12updateRoutingTable\x18\x03 \x03(\x0b\x32,.contract_out.ContractOut.UpdateRoutingTable\x12\x38\n\nupdateArea\x18\x04 \x03(\x0b\x32$.contract_out.ContractOut.UpdateArea\x1a;\n\x05Route\x12\x13\n\x0b\x64\x65stination\x18\x01 \x01(\x05\x12\x0f\n\x07nextHop\x18\x02 \x01(\x05\x12\x0c\n\x04\x64rop\x18\x03 \x01(\x08\x1a \n\x08Location\x12\t\n\x01x\x18\x01 \x01(\t\x12\t\n\x01y\x18\x02 \x01(\t\x1aJ\n\x04\x41rea\x12\x32\n\x06\x63\x65nter\x18\x01 \x01(\x0b\x32\".contract_out.ContractOut.Location\x12\x0e\n\x06radius\x18\x02 \x01(\t\x1a\x8f\x01\n\nUpdateUser\x12\x0f\n\x07relayId\x18\x01 \x01(\x05\x12\x0e\n\x06userId\x18\x02 \x01(\x05\x12\x30\n\x06\x61\x63tion\x18\x03 \x01(\x0e\x32 .contract_out.ContractOut.Action\x12.\n\x05\x65vent\x18\x04 \x01(\x0e\x32\x1f.contract_out.ContractOut.Event\x1a\x87\x01\n\x12UpdateRoutingTable\x12\x0f\n\x07relayId\x18\x01 \x01(\x05\x12.\n\x05route\x18\x02 \x01(\x0b\x32\x1f.contract_out.ContractOut.Route\x12\x30\n\x06\x61\x63tion\x18\x03 \x01(\x0e\x32 .contract_out.ContractOut.Action\x1aK\n\nUpdateArea\x12\x0f\n\x07relayId\x18\x01 \x01(\x05\x12,\n\x04\x61rea\x18\x02 \x01(\x0b\x32\x1e.contract_out.ContractOut.Area\")\n\x06\x41\x63tion\x12\x07\n\x03\x41\x44\x44\x10\x01\x12\n\n\x06\x44\x45LETE\x10\x02\x12\n\n\x06UPDATE\x10\x03\"!\n\x05\x45vent\x12\n\n\x06\x46ORCED\x10\x01\x12\x0c\n\x08REQUIRED\x10\x02')
)



_CONTRACTOUT_ACTION = _descriptor.EnumDescriptor(
  name='Action',
  full_name='contract_out.ContractOut.Action',
  filename=None,
  file=DESCRIPTOR,
  values=[
    _descriptor.EnumValueDescriptor(
      name='ADD', index=0, number=1,
      serialized_options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='DELETE', index=1, number=2,
      serialized_options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='UPDATE', index=2, number=3,
      serialized_options=None,
      type=None),
  ],
  containing_type=None,
  serialized_options=None,
  serialized_start=789,
  serialized_end=830,
)
_sym_db.RegisterEnumDescriptor(_CONTRACTOUT_ACTION)

_CONTRACTOUT_EVENT = _descriptor.EnumDescriptor(
  name='Event',
  full_name='contract_out.ContractOut.Event',
  filename=None,
  file=DESCRIPTOR,
  values=[
    _descriptor.EnumValueDescriptor(
      name='FORCED', index=0, number=1,
      serialized_options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='REQUIRED', index=1, number=2,
      serialized_options=None,
      type=None),
  ],
  containing_type=None,
  serialized_options=None,
  serialized_start=832,
  serialized_end=865,
)
_sym_db.RegisterEnumDescriptor(_CONTRACTOUT_EVENT)


_CONTRACTOUT_ROUTE = _descriptor.Descriptor(
  name='Route',
  full_name='contract_out.ContractOut.Route',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='destination', full_name='contract_out.ContractOut.Route.destination', index=0,
      number=1, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='nextHop', full_name='contract_out.ContractOut.Route.nextHop', index=1,
      number=2, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='drop', full_name='contract_out.ContractOut.Route.drop', index=2,
      number=3, type=8, cpp_type=7, label=1,
      has_default_value=False, default_value=False,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto2',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=257,
  serialized_end=316,
)

_CONTRACTOUT_LOCATION = _descriptor.Descriptor(
  name='Location',
  full_name='contract_out.ContractOut.Location',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='x', full_name='contract_out.ContractOut.Location.x', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='y', full_name='contract_out.ContractOut.Location.y', index=1,
      number=2, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto2',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=318,
  serialized_end=350,
)

_CONTRACTOUT_AREA = _descriptor.Descriptor(
  name='Area',
  full_name='contract_out.ContractOut.Area',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='center', full_name='contract_out.ContractOut.Area.center', index=0,
      number=1, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='radius', full_name='contract_out.ContractOut.Area.radius', index=1,
      number=2, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto2',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=352,
  serialized_end=426,
)

_CONTRACTOUT_UPDATEUSER = _descriptor.Descriptor(
  name='UpdateUser',
  full_name='contract_out.ContractOut.UpdateUser',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='relayId', full_name='contract_out.ContractOut.UpdateUser.relayId', index=0,
      number=1, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='userId', full_name='contract_out.ContractOut.UpdateUser.userId', index=1,
      number=2, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='action', full_name='contract_out.ContractOut.UpdateUser.action', index=2,
      number=3, type=14, cpp_type=8, label=1,
      has_default_value=False, default_value=1,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='event', full_name='contract_out.ContractOut.UpdateUser.event', index=3,
      number=4, type=14, cpp_type=8, label=1,
      has_default_value=False, default_value=1,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto2',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=429,
  serialized_end=572,
)

_CONTRACTOUT_UPDATEROUTINGTABLE = _descriptor.Descriptor(
  name='UpdateRoutingTable',
  full_name='contract_out.ContractOut.UpdateRoutingTable',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='relayId', full_name='contract_out.ContractOut.UpdateRoutingTable.relayId', index=0,
      number=1, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='route', full_name='contract_out.ContractOut.UpdateRoutingTable.route', index=1,
      number=2, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='action', full_name='contract_out.ContractOut.UpdateRoutingTable.action', index=2,
      number=3, type=14, cpp_type=8, label=1,
      has_default_value=False, default_value=1,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto2',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=575,
  serialized_end=710,
)

_CONTRACTOUT_UPDATEAREA = _descriptor.Descriptor(
  name='UpdateArea',
  full_name='contract_out.ContractOut.UpdateArea',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='relayId', full_name='contract_out.ContractOut.UpdateArea.relayId', index=0,
      number=1, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='area', full_name='contract_out.ContractOut.UpdateArea.area', index=1,
      number=2, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto2',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=712,
  serialized_end=787,
)

_CONTRACTOUT = _descriptor.Descriptor(
  name='ContractOut',
  full_name='contract_out.ContractOut',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='isACK', full_name='contract_out.ContractOut.isACK', index=0,
      number=1, type=8, cpp_type=7, label=2,
      has_default_value=False, default_value=False,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='updateUser', full_name='contract_out.ContractOut.updateUser', index=1,
      number=2, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='updateRoutingTable', full_name='contract_out.ContractOut.updateRoutingTable', index=2,
      number=3, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='updateArea', full_name='contract_out.ContractOut.updateArea', index=3,
      number=4, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[_CONTRACTOUT_ROUTE, _CONTRACTOUT_LOCATION, _CONTRACTOUT_AREA, _CONTRACTOUT_UPDATEUSER, _CONTRACTOUT_UPDATEROUTINGTABLE, _CONTRACTOUT_UPDATEAREA, ],
  enum_types=[
    _CONTRACTOUT_ACTION,
    _CONTRACTOUT_EVENT,
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto2',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=37,
  serialized_end=865,
)

_CONTRACTOUT_ROUTE.containing_type = _CONTRACTOUT
_CONTRACTOUT_LOCATION.containing_type = _CONTRACTOUT
_CONTRACTOUT_AREA.fields_by_name['center'].message_type = _CONTRACTOUT_LOCATION
_CONTRACTOUT_AREA.containing_type = _CONTRACTOUT
_CONTRACTOUT_UPDATEUSER.fields_by_name['action'].enum_type = _CONTRACTOUT_ACTION
_CONTRACTOUT_UPDATEUSER.fields_by_name['event'].enum_type = _CONTRACTOUT_EVENT
_CONTRACTOUT_UPDATEUSER.containing_type = _CONTRACTOUT
_CONTRACTOUT_UPDATEROUTINGTABLE.fields_by_name['route'].message_type = _CONTRACTOUT_ROUTE
_CONTRACTOUT_UPDATEROUTINGTABLE.fields_by_name['action'].enum_type = _CONTRACTOUT_ACTION
_CONTRACTOUT_UPDATEROUTINGTABLE.containing_type = _CONTRACTOUT
_CONTRACTOUT_UPDATEAREA.fields_by_name['area'].message_type = _CONTRACTOUT_AREA
_CONTRACTOUT_UPDATEAREA.containing_type = _CONTRACTOUT
_CONTRACTOUT.fields_by_name['updateUser'].message_type = _CONTRACTOUT_UPDATEUSER
_CONTRACTOUT.fields_by_name['updateRoutingTable'].message_type = _CONTRACTOUT_UPDATEROUTINGTABLE
_CONTRACTOUT.fields_by_name['updateArea'].message_type = _CONTRACTOUT_UPDATEAREA
_CONTRACTOUT_ACTION.containing_type = _CONTRACTOUT
_CONTRACTOUT_EVENT.containing_type = _CONTRACTOUT
DESCRIPTOR.message_types_by_name['ContractOut'] = _CONTRACTOUT
_sym_db.RegisterFileDescriptor(DESCRIPTOR)

ContractOut = _reflection.GeneratedProtocolMessageType('ContractOut', (_message.Message,), {

  'Route' : _reflection.GeneratedProtocolMessageType('Route', (_message.Message,), {
    'DESCRIPTOR' : _CONTRACTOUT_ROUTE,
    '__module__' : 'contract_out_pb2'
    # @@protoc_insertion_point(class_scope:contract_out.ContractOut.Route)
    })
  ,

  'Location' : _reflection.GeneratedProtocolMessageType('Location', (_message.Message,), {
    'DESCRIPTOR' : _CONTRACTOUT_LOCATION,
    '__module__' : 'contract_out_pb2'
    # @@protoc_insertion_point(class_scope:contract_out.ContractOut.Location)
    })
  ,

  'Area' : _reflection.GeneratedProtocolMessageType('Area', (_message.Message,), {
    'DESCRIPTOR' : _CONTRACTOUT_AREA,
    '__module__' : 'contract_out_pb2'
    # @@protoc_insertion_point(class_scope:contract_out.ContractOut.Area)
    })
  ,

  'UpdateUser' : _reflection.GeneratedProtocolMessageType('UpdateUser', (_message.Message,), {
    'DESCRIPTOR' : _CONTRACTOUT_UPDATEUSER,
    '__module__' : 'contract_out_pb2'
    # @@protoc_insertion_point(class_scope:contract_out.ContractOut.UpdateUser)
    })
  ,

  'UpdateRoutingTable' : _reflection.GeneratedProtocolMessageType('UpdateRoutingTable', (_message.Message,), {
    'DESCRIPTOR' : _CONTRACTOUT_UPDATEROUTINGTABLE,
    '__module__' : 'contract_out_pb2'
    # @@protoc_insertion_point(class_scope:contract_out.ContractOut.UpdateRoutingTable)
    })
  ,

  'UpdateArea' : _reflection.GeneratedProtocolMessageType('UpdateArea', (_message.Message,), {
    'DESCRIPTOR' : _CONTRACTOUT_UPDATEAREA,
    '__module__' : 'contract_out_pb2'
    # @@protoc_insertion_point(class_scope:contract_out.ContractOut.UpdateArea)
    })
  ,
  'DESCRIPTOR' : _CONTRACTOUT,
  '__module__' : 'contract_out_pb2'
  # @@protoc_insertion_point(class_scope:contract_out.ContractOut)
  })
_sym_db.RegisterMessage(ContractOut)
_sym_db.RegisterMessage(ContractOut.Route)
_sym_db.RegisterMessage(ContractOut.Location)
_sym_db.RegisterMessage(ContractOut.Area)
_sym_db.RegisterMessage(ContractOut.UpdateUser)
_sym_db.RegisterMessage(ContractOut.UpdateRoutingTable)
_sym_db.RegisterMessage(ContractOut.UpdateArea)


# @@protoc_insertion_point(module_scope)
