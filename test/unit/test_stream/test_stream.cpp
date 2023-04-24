#include "gtest/gtest.h"
#include "glog/logging.h"
#include "stream.h"
#include "transfer.h"
#include "defs.h"

// class TokenStream<std::string> {
struct TestToken {
	std::string token;
	int line;
	int column;
	TestToken() : token(""), line(0), column(0) {}
	TestToken(const std::string& token, int line, int column) : token(token), line(line), column(column) {}
	friend std::ostream& operator<<(std::ostream& os, const TestToken& token) {
		os << VisableString(token.token) << " " 
			<< token.line << " " 
			<< token.column;
		return os;
	}
	friend std::istream& operator>>(std::istream& is, TestToken& token) {
		is >> token.token >> token.line >> token.column;
		token.token = AsciiString(token.token);
		return is;
	}
	bool operator==(const TestToken& other) const {
		return token == other.token && line == other.line && column == other.column;
	}
};

TEST(TestStream, TestStream) {
	TokenStream<TestToken> stream;
	stream.AddToken(TestToken("a", 1, 1));
	stream.AddToken(TestToken("b", 1, 2));
	stream.AddToken(TestToken("   ", 1, 3));
	stream.AddToken(TestToken("     \
		 ", 1, 4));
		
	stream.AddToken(TestToken("你好", 2, 1));

	stream.ExportFile(std::string(PROJECT_ROOT_DIR) + "log/stream/test.txt");

	TokenStream<TestToken> stream2;
	stream2.LoadFile(std::string(PROJECT_ROOT_DIR) + "log/stream/test.txt");

	TestToken token;

	stream2.GetToken(token);
	EXPECT_EQ(token, TestToken("a", 1, 1));

	stream2.GetToken(token);
	EXPECT_EQ(token, TestToken("b", 1, 2));

	stream2.GetToken(token);
	EXPECT_EQ(token, TestToken("   ", 1, 3));

	stream2.GetToken(token);
	EXPECT_EQ(token, TestToken("     \
		 ", 1, 4));

	stream2.GetToken(token);
	EXPECT_EQ(token, TestToken("你好", 2, 1));

	EXPECT_EQ(stream2.Eof(), true);
	stream2.SetPosition(0);

	while(!stream2.Eof()) {
		stream2.GetToken(token);
	}

}