#include <optica/optica.hpp>

struct TaskType : public optica::Option<int, TaskType> {
  static constexpr std::string_view name = "Sosal";
  static constexpr bool required = true;
};

int main(int argc, char *argv[]) {
  optica::Parser<TaskType> parser;
  parser.Parse(argc, argv);
  parser.PrintMeta();
  auto value = parser.Get<TaskType>();
  if (value) {
    std::println("Value: {}", value.value());
  }
  return 0;
}
