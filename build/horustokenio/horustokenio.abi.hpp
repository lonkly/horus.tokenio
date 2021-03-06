const char* const horustokenio_abi = R"=====(
{
   "version": "eosio::abi/1.0",
   "types": [{
      "new_type_name": "account_name",
      "type": "name"
   },{
      "new_type_name": "time",
      "type": "uint32"
   }],
  "structs": [{
      "name": "transfer",
      "base": "",
      "fields": [
        {"name":"from", "type":"account_name"},
        {"name":"to", "type":"account_name"},
        {"name":"quantity", "type":"asset"},
        {"name":"memo", "type":"string"}
      ]
    },{
     "name": "create",
     "base": "",
     "fields": [
        {"name":"issuer", "type":"account_name"},
        {"name":"maximum_supply", "type":"asset"}
     ]
  },{
     "name": "issue",
     "base": "",
     "fields": [
        {"name":"to", "type":"account_name"},
        {"name":"quantity", "type":"asset"},
        {"name":"memo", "type":"string"}
     ]
  },{
     "name": "user_resources",
     "base": "",
     "fields": [
        {"name":"owner", "type":"account_name"},
        {"name":"total_staked_horus", "type":"asset"}
      ]
  },{
     "name": "staked_horus",
     "base": "",
     "fields": [
        {"name":"id", "type":"uint64"},
        {"name":"from", "type":"account_name"},
        {"name":"to", "type":"account_name"},
        {"name":"horus_weight", "type":"asset"},
        {"name":"time_initial", "type":"time"}
     ]
  },{
     "name": "stakehorus",
     "base": "",
     "fields": [
        {"name":"from", "type":"account_name"},
        {"name":"receiver", "type":"account_name"},
        {"name":"stake_horus_quantity", "type":"asset"}
     ]
  },{
     "name": "unstakehorus",
     "base": "",
     "fields": [
        {"name":"from", "type":"account_name"},
        {"name":"stake_id", "type":"uint64"}
     ]
  },{
     "name": "refund_request",
     "base": "",
     "fields": [
        {"name":"owner", "type":"account_name"},
        {"name":"request_time", "type":"time"},
        {"name":"horus_amount", "type":"asset"}
     ]
  },{
     "name": "refund_requests",
     "base": "",
     "fields": [
        {"name":"id", "type":"uint64"},
        {"name":"from", "type":"account_name"},
        {"name":"to", "type":"account_name"},
        {"name":"horus_amount", "type":"asset"},
        {"name":"request_time", "type":"time"}
     ]
  },{
     "name": "refundbyid",
     "base": "",
     "fields": [
        {"name":"owner", "type":"account_name"},
        {"name":"refund_id", "type":"uint64"}
     ]
  },{
     "name": "refundhorus",
     "base": "",
     "fields": [
        {"name":"owner", "type":"account_name"}
     ]
  },{
     "name": "claimreward",
     "base": "",
     "fields": [
        {"name":"owner", "type":"account_name"},
        {"name":"stake_id", "type":"uint64"}
     ]
  },{
     "name": "rmtoken",
     "base": "",
     "fields": [
        {"name":"sym", "type":"string"}
     ]
  },{
      "name": "account",
      "base": "",
      "fields": [
        {"name":"balance", "type":"asset"}
      ]
    },{
      "name": "currency_stats",
      "base": "",
      "fields": [
        {"name":"supply", "type":"asset"},
        {"name":"max_supply", "type":"asset"},
        {"name":"issuer", "type":"account_name"}
      ]
    }
  ],
  "actions": [{
      "name": "transfer",
      "type": "transfer",
      "ricardian_contract": ""
    },{
      "name": "issue",
      "type": "issue",
      "ricardian_contract": ""
    },{
      "name": "create",
      "type": "create",
      "ricardian_contract": ""
    },{
      "name": "stakehorus",
      "type": "stakehorus",
      "ricardian_contract": "# Action - {{ stakehorus }}
        ## Description
        The intent of the {{ stakehorus }} action is to stake HORUS tokens for access to the HorusPay Payroll Portal database and to mint the digital asset ECASH.

        As a HORUS token holder I {{ from }} wish to stake {{ stake_horus_quantity }} to {{ receiver }} granting the {{ receiver }} access to claim staking rewards.  A minimum of 10.0000 HORUS must be staked to participate in staking rewards.  Rewards are earned by staking HORUS tokens for longer than 7 days.  After 7 days payouts can be claimed in the form of ECASH.  ECASH is rewarded proportionally to the amount of HORUS staked and will be issued at a 1% monthly rate of the staked HORUS for a stake greater than or equal to 1 million HORUS, and at a 0.1% monthly rate for any stake less than 1 million.  In the future when the HorusPay Payroll Portal is fully released, the 1 million HORUS token threshold will be removed and all stakes will mint ECASH at a 0.1% monthly rate.  After the 7 day vesting period the staking durration will automatically be renewed for the owner {{ from }} to continue claiming rewards.  This action may change at a later point in time."
    },{
      "name": "unstakehorus",
      "type": "unstakehorus",
      "ricardian_contract": "# Action - {{ unstakehorus }}
      ## Description
      The intent of the {{ unstakehorus }} action is to unstake existing HORUS tokens.

      As a HORUS token staker I {{ from }} wish to unstake {{ stake_id }} for the receiver {{ staked_horus:to }}.  After a 7 day waiting period the HORUS tokens previously staked will become liquid for I {{ staked_horus:from }} to use again.  This action may change at a later point in time."
    },{
      "name": "refundhorus",
      "type": "refundhorus",
      "ricardian_contract": "# Action - {{ refundhorus }}
      ## Description
      The intent of the {{ refundhorus }} action is to give the {{ staked_horus:owner }} full liquid control of their HORUS tokens after the refund delay has finished.  refundhorus is a delayed transaction that will be called 7 days after unstakehorus is completed.  If for any reason this delayed transaction fails refundhorus can be called manually.  This action may change at a later point in time."
    },{
      "name": "claimreward",
      "type": "claimreward",
      "ricardian_contract": "# Action - {{ claimreward }}
      ## Description
      The intent of the {{ claimreward }} action is allow the {{ staked_horus:owner }} to claim ECASH for a specific {{ staked_horus:id }} that has surpassed the vesting period.  As a staking reward the digital asset ECASH will be issued at a 1% monthly rate of the staked HORUS for a stake greater than or equal to 1 million HORUS and at a 0.1% monthly rate for any stake less than 1 million.  In the future when the HorsuPay Payroll Portal is fully released, the 1 million HORUS token threshold will be removed and all stakes will mint ECASH at a 0.1% monthly rate.  After the 7 day vesting period the staking durration will automatically be renewed for the owner {{ from }} to continue claiming rewards.  This action may change at a later point in time."
    },{
      "name": "refundbyid",
      "type": "refundbyid",
      "ricardian_contract": ""
    },{
      "name": "rmtoken",
      "type": "rmtoken",
      "ricardian_contract": ""
    }
  ],
  "tables": [{
      "name": "accounts",
      "type": "account",
      "index_type": "i64",
      "key_names" : ["currency"],
      "key_types" : ["uint64"]
    },{
      "name": "stat",
      "type": "currency_stats",
      "index_type": "i64",
      "key_names" : ["currency"],
      "key_types" : ["uint64"]
    },{
      "name": "stakedhorus",
      "type": "staked_horus",
      "index_type": "i64",
      "key_names" : ["id"],
      "key_types" : ["uint64"]
    },{
      "name": "userres",
      "type": "user_resources",
      "index_type": "i64",
      "key_names" : ["owner"],
      "key_types" : ["uint64"]
    },{
      "name": "refunds",
      "type": "refund_request",
      "index_type": "i64",
      "key_names" : ["owner"],
      "key_types" : ["uint64"]
    },{
      "name": "horusrefunds",
      "type": "refund_requests",
      "index_type": "i64",
      "key_names" : ["owner"],
      "key_types" : ["uint64"]
    }
  ],
  "ricardian_clauses": [],
  "abi_extensions": []
}
)=====";
