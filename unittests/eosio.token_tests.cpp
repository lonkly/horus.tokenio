#include <boost/test/unit_test.hpp>
#include <eosio/testing/tester.hpp>
#include <eosio/chain/abi_serializer.hpp>

#include <horustokenio/horustokenio.wast.hpp>
#include <horustokenio/horustokenio.abi.hpp>

#include <Runtime/Runtime.h>

#include <fc/variant_object.hpp>

using namespace eosio::testing;
using namespace eosio;
using namespace eosio::chain;
using namespace eosio::testing;
using namespace fc;
using namespace std;

using mvo = fc::mutable_variant_object;

class horustokenio_tester : public tester {
public:

   horustokenio_tester() {
      produce_blocks( 2 );

      create_accounts( { N(alice), N(bob), N(carol), N(horustokenio) } );
      produce_blocks( 2 );

      set_code( N(horustokenio), horustokenio_wast );
      set_abi( N(horustokenio), horustokenio_abi );

      produce_blocks();

      const auto& accnt = control->db().get<account_object,by_name>( N(horustokenio) );
      abi_def abi;
      BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(accnt.abi, abi), true);
      abi_ser.set_abi(abi, abi_serializer_max_time);
   }

   action_result push_action( const account_name& signer, const action_name &name, const variant_object &data ) {
      string action_type_name = abi_ser.get_action_type(name);

      action act;
      act.account = N(horustokenio);
      act.name    = name;
      act.data    = abi_ser.variant_to_binary( action_type_name, data, abi_serializer_max_time );

      return base_tester::push_action( std::move(act), uint64_t(signer));
   }


   fc::variant get_stats( const string& symbolname )
   {
      auto symb = eosio::chain::symbol::from_string(symbolname);
      auto symbol_code = symb.to_symbol_code().value;
      vector<char> data = get_row_by_account( N(horustokenio), symbol_code, N(stat), symbol_code );
      return data.empty() ? fc::variant() : abi_ser.binary_to_variant( "currency_stats", data, abi_serializer_max_time );
   }

   fc::variant get_account( account_name acc, const string& symbolname)
   {
      auto symb = eosio::chain::symbol::from_string(symbolname);
      auto symbol_code = symb.to_symbol_code().value;
      vector<char> data = get_row_by_account( N(horustokenio), acc, N(accounts), symbol_code );
      return data.empty() ? fc::variant() : abi_ser.binary_to_variant( "account", data, abi_serializer_max_time );
   }

   fc::variant get_horus_stake( account_name to, const uint64_t& id )
   {
      vector<char> data = get_row_by_account( N(horustokenio), to, N(stakedhorus), id );
      return data.empty() ? fc::variant() : abi_ser.binary_to_variant( "staked_horus", data, abi_serializer_max_time );
   }

   /*************************************************************************
   *                            A C T I O N S
   **************************************************************************/

   action_result create( account_name issuer,
                asset        maximum_supply ) {

      return push_action( N(horustokenio), N(create), mvo()
           ( "issuer", issuer)
           ( "maximum_supply", maximum_supply)
      );
   }

   action_result issue( account_name issuer, account_name to, asset quantity, string memo ) {
      return push_action( issuer, N(issue), mvo()
           ( "to", to)
           ( "quantity", quantity)
           ( "memo", memo)
      );
   }

   action_result transfer( account_name from,
                  account_name to,
                  asset        quantity,
                  string       memo ) {
      return push_action( from, N(transfer), mvo()
           ( "from", from)
           ( "to", to)
           ( "quantity", quantity)
           ( "memo", memo)
      );
   }

   action_result stakehorus( account_name from,
                             account_name receiver,
                             asset        stake_horus_quantity,
                             bool         transfer ) {
      return push_action( from, N(stakehorus), mvo()
           ( "from", from)
           ( "receiver", receiver)
           ( "stake_horus_quantity", stake_horus_quantity)
           ( "transfer", transfer)
      );
   }

   action_result unstakehorus( account_name from,
                               uint64_t     unstake_id ) {
      return push_action( from, N(unstakehorus), mvo()
           ( "from", from)
           ( "unstake_id", unstake_id)
      );
   }

   abi_serializer abi_ser;
};


/*************************************************************************
*                            T E S T S
**************************************************************************/

BOOST_AUTO_TEST_SUITE(eosio_token_tests)


/*************************************************************************
* Test creatng a new token called TKN
**************************************************************************/
BOOST_FIXTURE_TEST_CASE( create_tests, horustokenio_tester ) try {

   auto token = create( N(horustokenio), asset::from_string("1000.0000 TKN"));
   auto stats = get_stats("4,TKN");
   REQUIRE_MATCHING_OBJECT( stats, mvo()
      ("supply", "0.0000 TKN")
      ("max_supply", "1000.0000 TKN")
      ("issuer", "horustokenio")
   );
   produce_blocks(1);

} FC_LOG_AND_RETHROW()


/*************************************************************************
* Test creating a token with a negative supply
**************************************************************************/
BOOST_FIXTURE_TEST_CASE( create_negative_max_supply, horustokenio_tester ) try {

   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "max-supply must be positive" ),
      create( N(horustokenio), asset::from_string("-1000.0000 TKN"))
   );

} FC_LOG_AND_RETHROW()


/*************************************************************************
* Test you cannot create two tokens with the same symbol
**************************************************************************/
BOOST_FIXTURE_TEST_CASE( symbol_already_exists, horustokenio_tester ) try {

   auto token = create( N(horustokenio), asset::from_string("100 TKN"));
   auto stats = get_stats("0,TKN");
   REQUIRE_MATCHING_OBJECT( stats, mvo()
      ("supply", "0 TKN")
      ("max_supply", "100 TKN")
      ("issuer", "horustokenio")
   );
   produce_blocks(1);

   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "token with symbol already exists" ),
                        create( N(horustokenio), asset::from_string("100 TKN"))
   );

} FC_LOG_AND_RETHROW()


/*************************************************************************
* Test max supply of token
**************************************************************************/
BOOST_FIXTURE_TEST_CASE( create_max_supply, horustokenio_tester ) try {

   auto token = create( N(horustokenio), asset::from_string("4611686018427387903 TKN"));
   auto stats = get_stats("0,TKN");
   REQUIRE_MATCHING_OBJECT( stats, mvo()
      ("supply", "0 TKN")
      ("max_supply", "4611686018427387903 TKN")
      ("issuer", "horustokenio")
   );
   produce_blocks(1);

   asset max(10, symbol(SY(0, NKT)));
   share_type amount = 4611686018427387904;
   static_assert(sizeof(share_type) <= sizeof(asset), "asset changed so test is no longer valid");
   static_assert(std::is_trivially_copyable<asset>::value, "asset is not trivially copyable");
   memcpy(&max, &amount, sizeof(share_type)); // hack in an invalid amount

   BOOST_CHECK_EXCEPTION( create( N(horustokenio), max) , asset_type_exception, [](const asset_type_exception& e) {
      return expect_assert_message(e, "magnitude of asset amount must be less than 2^62");
   });

} FC_LOG_AND_RETHROW()


/*************************************************************************
* Test creating token with max decimal places
**************************************************************************/
BOOST_FIXTURE_TEST_CASE( create_max_decimals, horustokenio_tester ) try {

   auto token = create( N(horustokenio), asset::from_string("1.000000000000000000 TKN"));
   auto stats = get_stats("18,TKN");
   REQUIRE_MATCHING_OBJECT( stats, mvo()
      ("supply", "0.000000000000000000 TKN")
      ("max_supply", "1.000000000000000000 TKN")
      ("issuer", "horustokenio")
   );
   produce_blocks(1);

   asset max(10, symbol(SY(0, NKT)));
   //1.0000000000000000000 => 0x8ac7230489e80000L
   share_type amount = 0x8ac7230489e80000L;
   static_assert(sizeof(share_type) <= sizeof(asset), "asset changed so test is no longer valid");
   static_assert(std::is_trivially_copyable<asset>::value, "asset is not trivially copyable");
   memcpy(&max, &amount, sizeof(share_type)); // hack in an invalid amount

   BOOST_CHECK_EXCEPTION( create( N(horustokenio), max) , asset_type_exception, [](const asset_type_exception& e) {
      return expect_assert_message(e, "magnitude of asset amount must be less than 2^62");
   });

} FC_LOG_AND_RETHROW()


/*************************************************************************
* Test issuing token
**************************************************************************/
BOOST_FIXTURE_TEST_CASE( issue_tests, horustokenio_tester ) try {

   auto token = create( N(horustokenio), asset::from_string("1000.0000 TKN"));
   produce_blocks(1);

   issue( N(horustokenio), N(horustokenio), asset::from_string("500.0000 TKN"), "hola" );

   auto stats = get_stats("4,TKN");
   REQUIRE_MATCHING_OBJECT( stats, mvo()
      ("supply", "500.0000 TKN")
      ("max_supply", "1000.0000 TKN")
      ("issuer", "horustokenio")
   );

   auto horustokenio_balance = get_account(N(horustokenio), "3,TKN");
   REQUIRE_MATCHING_OBJECT( horustokenio_balance, mvo()
      ("balance", "500.0000 TKN")
   );

   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "quantity exceeds available supply" ),
      issue( N(horustokenio), N(horustokenio), asset::from_string("500.0001 TKN"), "hola" )
   );

   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "must issue positive quantity" ),
      issue( N(horustokenio), N(horustokenio), asset::from_string("-1.0000 TKN"), "hola" )
   );

   BOOST_REQUIRE_EQUAL( success(),
      issue( N(horustokenio), N(horustokenio), asset::from_string("1.0000 TKN"), "hola" )
   );

} FC_LOG_AND_RETHROW()


/*************************************************************************
* Test transfering token
**************************************************************************/
BOOST_FIXTURE_TEST_CASE( transfer_tests, horustokenio_tester ) try {

   auto token = create( N(horustokenio), asset::from_string("1000 CERO"));
   produce_blocks(1);

   issue( N(horustokenio), N(horustokenio), asset::from_string("1000 CERO"), "hola" );

   auto stats = get_stats("0,CERO");
   REQUIRE_MATCHING_OBJECT( stats, mvo()
      ("supply", "1000 CERO")
      ("max_supply", "1000 CERO")
      ("issuer", "horustokenio")
   );

   auto horustokenio_balance = get_account(N(horustokenio), "0,CERO");
   REQUIRE_MATCHING_OBJECT( horustokenio_balance, mvo()
      ("balance", "1000 CERO")
   );

   transfer( N(horustokenio), N(bob), asset::from_string("300 CERO"), "hola" );

   horustokenio_balance = get_account(N(horustokenio), "0,CERO");
   REQUIRE_MATCHING_OBJECT( horustokenio_balance, mvo()
      ("balance", "700 CERO")
   );

   auto bob_balance = get_account(N(bob), "0,CERO");
   REQUIRE_MATCHING_OBJECT( bob_balance, mvo()
      ("balance", "300 CERO")
   );

   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "overdrawn balance" ),
      transfer( N(horustokenio), N(bob), asset::from_string("701 CERO"), "hola" )
   );

   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "must transfer positive quantity" ),
      transfer( N(horustokenio), N(bob), asset::from_string("-1000 CERO"), "hola" )
   );

} FC_LOG_AND_RETHROW()


/*************************************************************************
* Test staking HORUS tokens
**************************************************************************/
BOOST_FIXTURE_TEST_CASE( stakehorus_tests, horustokenio_tester ) try {

   auto token = create( N(horustokenio), asset::from_string("1000.0000 HORUS"));
   produce_blocks(1);

   issue( N(horustokenio), N(horustokenio), asset::from_string("1000.0000 HORUS"), "issuing HORUS" );

   // Give tokens to alice
   transfer( N(horustokenio), N(alice), asset::from_string("500.0000 HORUS"), "transfer to alice" );

   // alice stake HORUS tokens for herself
   stakehorus( N(alice), N(alice), asset::from_string("100.0000 HORUS"), false );

   auto alice_stake = get_horus_stake( N(alice), 0 );
   REQUIRE_MATCHING_OBJECT( alice_stake, mvo()
      ("id", "0")
      ("from", "alice")
      ("to", "alice")
      ("horus_weight", "100.0000 HORUS")
      ("time_initial", "1577836805")
   );

   auto alice_balance = get_account(N(alice), "4,HORUS");
   REQUIRE_MATCHING_OBJECT( alice_balance, mvo()
      ("balance", "400.0000 HORUS")
   );


   // alice staking HORUS for bob
   stakehorus( N(alice), N(bob), asset::from_string("100.0000 HORUS"), false );

   alice_stake = get_horus_stake( N(alice), 1 );
   REQUIRE_MATCHING_OBJECT( alice_stake, mvo()
      ("id", "1")
      ("from", "alice")
      ("to", "bob")
      ("horus_weight", "100.0000 HORUS")
      ("time_initial", "1577836805")
   );

   alice_balance = get_account(N(alice), "4,HORUS");
   REQUIRE_MATCHING_OBJECT( alice_balance, mvo()
      ("balance", "300.0000 HORUS")
   );


   // minimum balace error
   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "minimum stake required is '0.1000 HORUS'" ),
      stakehorus( N(alice), N(alice), asset::from_string("0.0999 HORUS"), false )
   );

   // attempt to stake more HORUS tokens then user has
   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "overdrawn balance" ),
      stakehorus( N(alice), N(alice), asset::from_string("301.0000 HORUS"), false )
   );

   // stake must be positive amount
   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "must stake a positive amount" ),
      stakehorus( N(alice), N(alice), asset::from_string("-100.0000 HORUS"), false )
   );

} FC_LOG_AND_RETHROW()


/*************************************************************************
* Test unstaking HORUS tokens
**************************************************************************/
BOOST_FIXTURE_TEST_CASE( unstakehorus_tests, horustokenio_tester ) try {

   auto token = create( N(horustokenio), asset::from_string("1000.0000 HORUS"));
   produce_blocks(1);

   issue( N(horustokenio), N(horustokenio), asset::from_string("1000.0000 HORUS"), "issuing HORUS" );

   // Give tokens to alice
   transfer( N(horustokenio), N(alice), asset::from_string("500.0000 HORUS"), "transfer to alice" );

   // alice stake HORUS tokens for herself
   stakehorus( N(alice), N(alice), asset::from_string("100.0000 HORUS"), false );

   auto alice_stake = get_horus_stake( N(alice), 0 );
   REQUIRE_MATCHING_OBJECT( alice_stake, mvo()
      ("id", "0")
      ("from", "alice")
      ("to", "alice")
      ("horus_weight", "100.0000 HORUS")
      ("time_initial", "1577836805")
   );

   auto alice_balance = get_account(N(alice), "4,HORUS");
   REQUIRE_MATCHING_OBJECT( alice_balance, mvo()
      ("balance", "400.0000 HORUS")
   );


} FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
