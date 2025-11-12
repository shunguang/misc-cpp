int test_mat_part1(int argc, char* argv[]);
int test_mat_part2(int argc, char* argv[]);
int test_mat_part3(int argc, char* argv[]);
int test_mat_part4(int argc, char* argv[]);
int test_mat_tools1(int argc, char* argv[]);
int test_mat_tools2(int argc, char* argv[]);
int test_mat_tools3(int argc, char* argv[]);
int test_mat_tools4(int argc, char* argv[]);

int test_lu(int argc, char* argv[]);
int test_line2d(int argc, char* argv[]);
int test_random(int argc, char* argv[]);
int test_svd(int argc, char* argv[]);

int main(int argc, char* argv[])
{
	test_mat_part1( argc, argv);
	test_mat_part2(argc, argv);
	test_mat_part3(argc, argv);
	test_mat_part4(argc, argv);
	test_lu(argc, argv);
	test_line2d(argc, argv);
	
	test_mat_tools1(argc, argv);
	test_mat_tools2(argc, argv);
	test_mat_tools3(argc, argv);
	test_mat_tools4(argc, argv);

	test_random(argc, argv);
	test_svd(argc, argv);

	return 0;
}

