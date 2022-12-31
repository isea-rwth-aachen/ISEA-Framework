from conans import ConanFile, CMake


class ISEAFrameworkConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = (
        "boost/1.78.0",
        "eigen/3.4.0",
        'matio/1.5.21',
    )  # comma-separated list of requirements
    generators = "cmake_find_package_multi","cmake_find_package"
    default_options = {"boost:shared": False}

    def imports(self):
        self.copy("*.dll", dst="bin", src="bin")  # From bin to bin
        self.copy("*.dylib*", dst="bin", src="lib")  # From lib to bin
