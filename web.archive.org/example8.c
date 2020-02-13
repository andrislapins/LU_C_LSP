main()
{
    int n = 0;
    float x = 0.0;

    create_int_dialog_entry("n", &n);
    create_float_dialog_entry("x", &x);

    set_up_dialog("Setup", 0, 0);

    read_dialog_window();

    printf("n = %d, x = %f\n", n, x);
}

// use Cfgx alias - means what and how?

// What is kconfig / kconfig files?
