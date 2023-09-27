#include "svc-database.h"

int main()
{
  SimpleVoiceChat::SVCdb db(boost::filesystem::exists(SVC_DATABASE_NAME));

  return 0;
}
