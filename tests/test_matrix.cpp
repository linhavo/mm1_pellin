/*!
 * @file 		test_matrix.cpp
 * @author 		Zdenek Travnicek <travnicek@iim.cz>
 * @date 		10. 5. 2016
 * @copyright	Institute of Intermedia, CTU in Prague, 2013
 * 				Distributed under BSD Licence, details in file doc/LICENSE
 *
 */

#include "iimavlib/catch/catch.hpp"
#include "iimavlib/ArrayTypes.h"

namespace iimavlib {
namespace {
const std::vector<int> seq_2_2 {0, 1, 2, 3};
const std::vector<int> seq_0_8 {0, 1, 2, 3, 4, 5, 6, 7, 8};
const std::vector<int> seq_0_9 {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

const std::vector<int> seq_0_8_trans_3_3 {0, 3, 6, 1, 4, 7, 2, 5, 8};
const std::vector<int> seq_0_9_trans_2_5 {0, 5, 1, 6, 2, 7, 3, 8, 4, 9};
const std::vector<int> seq_0_9_trans_5_2 {0, 2, 4, 6, 8, 1, 3, 5, 7, 9};

const std::vector<int> seq_2_2_mult_2_2 {2, 3, 6, 11};
const std::vector<int> seq_3_3_mult_1_3 {5, 14, 23};
}
TEST_CASE("Matrix") {
	SECTION("Initialization") {
		SECTION("default ctor") {
			matrix<int> m;
			REQUIRE(m.data().size() == 0);
		}
		SECTION("other ctors") {
			SECTION("10x1") {
				matrix<int> m1(10);
				REQUIRE(m1.data().size() == 10);
				m1.sequence();
				REQUIRE(m1.data() == seq_0_9);
				matrix<int> m3(seq_0_9, 10);
				REQUIRE(m1.data() == m3.data());
				auto m2 = m1.transpose();
				REQUIRE(m2.data() == seq_0_9);
				m1.self_transpose();
				REQUIRE(m1.data() == seq_0_9);
				REQUIRE(m1.data() == m2.data());
			}
			SECTION("1x10") {
				matrix<int> m1(1, 10);
				REQUIRE(m1.data().size() == 10);
				m1.sequence();
				REQUIRE(m1.data() == seq_0_9);
				matrix<int> m3(seq_0_9, 1, 10);
				REQUIRE(m1.data() == m3.data());
				auto m2 = m1.transpose();
				REQUIRE(m2.data() == seq_0_9);
				m1.self_transpose();
				REQUIRE(m1.data() == seq_0_9);
				REQUIRE(m1.data() == m2.data());
			}
			SECTION("3x3") {
				matrix<int> m1(3, 3);
				REQUIRE(m1.data().size() == 9);
				m1.sequence();
				REQUIRE(m1.data() == seq_0_8);
				matrix<int> m3(seq_0_8, 3, 3);
				REQUIRE(m1.data() == m3.data());
				auto m2 = m1.transpose();
				REQUIRE(m2.data() == seq_0_8_trans_3_3);
				m1.self_transpose();
				REQUIRE(m1.data() == seq_0_8_trans_3_3);
				REQUIRE(m1.data() == m2.data());
			}
			SECTION("2x5") {
				matrix<int> m1(2, 5);
				REQUIRE(m1.data().size() == 10);
				m1.sequence();
				REQUIRE(m1.data() == seq_0_9);
				matrix<int> m3(seq_0_9, 2, 5);
				REQUIRE(m1.data() == m3.data());
				auto m2 = m1.transpose();
				REQUIRE(m2.data() == seq_0_9_trans_2_5);
				m1.self_transpose();
				REQUIRE(m1.data() == seq_0_9_trans_2_5);
				REQUIRE(m1.data() == m2.data());
			}
			SECTION("5x2") {
				matrix<int> m1(5, 2);
				REQUIRE(m1.data().size() == 10);
				m1.sequence();
				REQUIRE(m1.data() == seq_0_9);
				matrix<int> m3(seq_0_9, 5, 2);
				REQUIRE(m1.data() == m3.data());
				auto m2 = m1.transpose();
				REQUIRE(m2.data() == seq_0_9_trans_5_2);
				m1.self_transpose();
				REQUIRE(m1.data() == seq_0_9_trans_5_2);
				REQUIRE(m1.data() == m2.data());

			}
		}
	}
	SECTION("Operations") {

		SECTION("Multiplication") {
			SECTION("2x2 * 2x2") {
				matrix<int> mat_2_2 (2,2);
				mat_2_2.sequence();
				auto res = mat_2_2 * mat_2_2;
				REQUIRE(res.data() == seq_2_2_mult_2_2);
			}
			SECTION("3x3 * 3") {
				matrix<int> mat_3_3 (3, 3);
				mat_3_3.sequence();
				simplearray_t<int> simple_1_3 = {0, 1, 2};
				auto res = mat_3_3 * simple_1_3;
				REQUIRE(res.data() == seq_3_3_mult_1_3);
			}
		}
	}

}


}
