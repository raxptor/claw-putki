
// generated.
namespace inki
{
	void bind_claw();
	void bind_ccg_ui();
}

int run_putki_datatool(int argc, char **argv);

int main(int argc, char **argv)
{
	inki::bind_claw();
	inki::bind_ccg_ui();
	return run_putki_datatool(argc, argv);
}
