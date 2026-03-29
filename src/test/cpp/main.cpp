/*!
 * @brief
 * @param argc number of string arguments passed via argv
 * @param argv array to command line arguments as strings
 *             (an array of character pointers)
 * @return the completion status code (zero indicates success)
 */
int main([[maybe_unused]] int argc, [[maybe_unused]] char ** argv)
{
    try
    {
    }
    catch (const std::exception & e)
    {
        std::cerr << e.what() << std::endl;
        return (EXIT_FAILURE);
    }
    catch (...)
    {
        std::cerr << "Unknown exception occured" << std::endl;
        return (EXIT_FAILURE);
    }

    return (runStatus == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}
