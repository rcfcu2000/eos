const char* const coinxp_latestprice_abi = R"=====(
{
  "____comment": "This file was generated by eosio-abigen. DO NOT EDIT - 2018-11-08T08:05:37",
  "version": "eosio::abi/1.0",
  "types": [],
  "structs": [{
      "name": "Price",
      "base": "",
      "fields": [{
          "name": "amount",
          "type": "uint64"
        },{
          "name": "price",
          "type": "float64"
        },{
          "name": "timestamp",
          "type": "uint64"
        }
      ]
    },{
      "name": "BuyOrder",
      "base": "",
      "fields": [{
          "name": "auto_key",
          "type": "uint64"
        },{
          "name": "request_id",
          "type": "uint64"
        },{
          "name": "price",
          "type": "float64"
        },{
          "name": "amount",
          "type": "int64"
        },{
          "name": "timestamp",
          "type": "uint64"
        },{
          "name": "user",
          "type": "name"
        },{
          "name": "exchange",
          "type": "name"
        },{
          "name": "bitmap",
          "type": "uint64"
        },{
          "name": "reserved0",
          "type": "uint64"
        },{
          "name": "reserved1",
          "type": "uint64"
        },{
          "name": "reserved2",
          "type": "uint64"
        },{
          "name": "reserved3",
          "type": "uint64"
        }
      ]
    },{
      "name": "SellOrder",
      "base": "",
      "fields": [{
          "name": "auto_key",
          "type": "uint64"
        },{
          "name": "request_id",
          "type": "uint64"
        },{
          "name": "price",
          "type": "float64"
        },{
          "name": "amount",
          "type": "int64"
        },{
          "name": "timestamp",
          "type": "uint64"
        },{
          "name": "user",
          "type": "name"
        },{
          "name": "exchange",
          "type": "name"
        },{
          "name": "bitmap",
          "type": "uint64"
        },{
          "name": "reserved0",
          "type": "uint64"
        },{
          "name": "reserved1",
          "type": "uint64"
        },{
          "name": "reserved2",
          "type": "uint64"
        },{
          "name": "reserved3",
          "type": "uint64"
        }
      ]
    },{
      "name": "ABuyBook",
      "base": "",
      "fields": [{
          "name": "key",
          "type": "uint64"
        },{
          "name": "price",
          "type": "float64"
        },{
          "name": "amount",
          "type": "int64"
        }
      ]
    },{
      "name": "ASellBook",
      "base": "",
      "fields": [{
          "name": "key",
          "type": "uint64"
        },{
          "name": "price",
          "type": "float64"
        },{
          "name": "amount",
          "type": "int64"
        }
      ]
    },{
      "name": "updateprice",
      "base": "",
      "fields": [{
          "name": "scope",
          "type": "uint64"
        },{
          "name": "amount",
          "type": "uint64"
        },{
          "name": "price",
          "type": "float64"
        }
      ]
    },{
      "name": "updatebook",
      "base": "",
      "fields": [{
          "name": "scope",
          "type": "uint64"
        }
      ]
    }
  ],
  "actions": [{
      "name": "updateprice",
      "type": "updateprice",
      "ricardian_contract": ""
    },{
      "name": "updatebook",
      "type": "updatebook",
      "ricardian_contract": ""
    }
  ],
  "tables": [{
      "name": "price",
      "index_type": "i64",
      "key_names": [
        "amount"
      ],
      "key_types": [
        "uint64"
      ],
      "type": "Price"
    },{
      "name": "buybook",
      "index_type": "i64",
      "key_names": [
        "auto_key"
      ],
      "key_types": [
        "uint64"
      ],
      "type": "BuyOrder"
    },{
      "name": "sellbook",
      "index_type": "i64",
      "key_names": [
        "auto_key"
      ],
      "key_types": [
        "uint64"
      ],
      "type": "SellOrder"
    },{
      "name": "abuybook",
      "index_type": "i64",
      "key_names": [
        "key"
      ],
      "key_types": [
        "uint64"
      ],
      "type": "ABuyBook"
    },{
      "name": "asellbook",
      "index_type": "i64",
      "key_names": [
        "key"
      ],
      "key_types": [
        "uint64"
      ],
      "type": "ASellBook"
    }
  ],
  "ricardian_clauses": [],
  "error_messages": [],
  "abi_extensions": [],
  "variants": []
})=====";
