// XmlIOServer
#include "xmlioserver.hpp"

#include "attribute_template_impl.hpp"
#include "group_template_impl.hpp"

#include "client.hpp"
#include "server.hpp"
extern "C"
{
   void xios_dtreatment_start(tree::CContext * context_hdl, int filetype, int comm_client_server, int comm_server);
   void xios_set_timestep(double ts_year, double ts_month,  double ts_day,
                          double ts_hour, double ts_minute, double ts_second);
   void xios_write_data_k83(const char * fieldid, int fieldid_size, double * data_k8, int data_Xsize, int data_Ysize, int data_Zsize);
   void xios_update_calendar(int step);
}
// Point d'entrée du programme principal

int main (int argc, char ** argv, char ** UNUSED (env))
{
   try
   {
      MPIComm comm_client, comm_client_server, comm_server;
      CXIOSManager::Initialise (CXIOSManager::CLIENT_SERVER, &argc, &argv);
      if (CMPIManager::GetCommRank() == 0)
      {

         CMPIManager ::DispatchClient(true, comm_client, comm_client_server,  comm_server);
         CXIOSManager::RunServer("Nemo", comm_client_server, comm_server); 
         CServer::CreateServer(comm_client_server)->run();
      }
      else
      {
         ARRAY_CREATE(data, double, 3, [20][30][20]);
         
         CMPIManager ::DispatchClient(false, comm_client, comm_client_server,  comm_server);
         CTreeManager::ParseFile("iodef.xml");
         CTreeManager::SetCurrentContextId("context1");
         boost::shared_ptr<CContext> context =
             CObjectFactory::GetObject<CContext>("context1");
         xios_set_timestep(0, 0, 0, 1, 0, 0);
         xios_dtreatment_start(context.get(), 0, comm_client_server, -1);

         for (int i = 0; i < 200; i++)
         {
            xios_update_calendar(i);
            for (int i = 0, l=0; i < 20; i++)
               for (int j = 0; j < 30; j++)
                  for (int k = 0; k < 20; k++, l++)   
                     (*data)[i][j][k] = l*10+i;
            xios_write_data_k83("champ1", 6, data->data(), 20, 30, 20);
         }
         CClient::GetClient()->finalize();
      }
      CXIOSManager::Finalize ();
         
   }
   catch (CException & exc)
   {
      std::cerr << exc.getMessage () << std::endl;
      CMPIManager::Finalize ();
      return (EXIT_FAILURE);
   }
   return (EXIT_SUCCESS);
}

