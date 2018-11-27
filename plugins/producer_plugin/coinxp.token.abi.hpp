const char* const coinxp_token_abi = R"=====(
{
  "____comment": "This file was generated by eosio-abigen. DO NOT EDIT - 2018-11-08T08:04:43",
  "version": "eosio::abi/1.0",
  "types": [{
      "new_type_name": "time_t",
      "type": "int32"
    }
  ],
  "structs": [{
      "name": "account_asset",
      "base": "",
      "fields": [{
          "name": "balance",
          "type": "asset"
        },{
          "name": "supply",
          "type": "asset"
        },{
          "name": "timestamp",
          "type": "time_t"
        }
      ]
    },{
      "name": "currency",
      "base": "",
      "fields": [{
          "name": "supply",
          "type": "asset"
        },{
          "name": "max_supply",
          "type": "asset"
        },{
          "name": "issuer",
          "type": "name"
        }
      ]
    },{
      "name": "tokens",
      "base": "",
      "fields": [{
          "name": "supply",
          "type": "asset"
        }
      ]
    },{
      "name": "create",
      "base": "",
      "fields": [{
          "name": "issuer",
          "type": "name"
        },{
          "name": "maxsupply",
          "type": "asset"
        }
      ]
    },{
      "name": "issue",
      "base": "",
      "fields": [{
          "name": "quantity",
          "type": "asset"
        },{
          "name": "memo",
          "type": "string"
        }
      ]
    },{
      "name": "tokenmap_s",
      "base": "",
      "fields": [{
          "name": "id",
          "type": "uint64"
        },{
          "name": "tokenSymbol",
          "type": "uint64"
        },{
          "name": "token",
          "type": "string"
        },{
          "name": "contractaddr",
          "type": "string"
        },{
          "name": "symbol",
          "type": "uint64"
        }
      ]
    },{
      "name": "oracle_rec",
      "base": "",
      "fields": [{
          "name": "username",
          "type": "name"
        }
      ]
    }
  ],
  "actions": [{
      "name": "create",
      "type": "create",
      "ricardian_contract": ""
    },{
      "name": "issue",
      "type": "issue",
      "ricardian_contract": ""
    }
  ],
  "tables": [{
      "name": "accountasset",
      "index_type": "i64",
      "key_names": [
        "balance"
      ],
      "key_types": [
        "asset"
      ],
      "type": "account_asset"
    },{
      "name": "currency",
      "index_type": "i64",
      "key_names": [
        "supply"
      ],
      "key_types": [
        "asset"
      ],
      "type": "currency"
    },{
      "name": "stat",
      "index_type": "i64",
      "key_names": [
        "supply"
      ],
      "key_types": [
        "asset"
      ],
      "type": "tokens"
    },{
      "name": "tokens",
      "index_type": "i64",
      "key_names": [
        "id"
      ],
      "key_types": [
        "uint64"
      ],
      "type": "tokenmap_s"
    },{
      "name": "oracles",
      "index_type": "i64",
      "key_names": [
        "username"
      ],
      "key_types": [
        "name"
      ],
      "type": "oracle_rec"
    }
  ],
  "ricardian_clauses": [],
  "error_messages": [],
  "abi_extensions": [],
  "variants": []
})=====";
