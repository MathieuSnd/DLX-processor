module decoder(input logic        clk,
               input logic        reset_n,
               input logic        ID,
               input logic [31:0] i_data_read,

               output logic d_write_enable,
               output logic d_load_enable,
               output logic Iv_alu,
               output logic Pc_alu,

               output logic [0:1] Pc_cmd,
               output logic [0:1] Pc_val,


               output logic [3:0] I,

               output logic [4:0] Rs1,
               output logic [4:0] Rs2,
               output logic [4:0] Rd,

               output logic [31:0] Iv);

        always@(posedge clk)
          if(!reset_n)
            begin
              d_write_enable <= '0;
              d_load_enable  <= '0;
              Iv_alu         <= '0;
              Pc_alu         <= '0;
              Pc_cmd         <= '0;
              Pc_val         <= '0;
              I              <= '0;
              Rs1            <= '0;
              Rs2            <= '0;
              Rd             <= '0;
              Iv             <= '0;
            end
          else
            if(i_data_read[31:26] == '0)
              begin
                case(i_data_read[5:0])
                  0x20    : I <= 4d'1;
                  0x24    : I <= 4d'3;
                  0x25    : I <= 4d'4;
                  0x28    : I <= 4d'10;
                  0x2c    : I <= 4d'11;
                  0x04    : I <= 4d'6;
                  0x2a    : I <= 4d'12;
                  0x29    : I <= 4d'13;
                  0x07    : I <= 4d'14;
                  0x06    : I <= 4d'7;
                  0x22    : I <= 4d'2;
                  0x26    : I <= 4d'5;
                  default : I <= 4d'0;
                endcase
                Rs1 <= i_data_read[25:21];
                Rs2 <= i_data_read[20:16];
                Rd  <= i_data_read[15:11];
                Pc_cmd <= '0;
                Iv_alu <= '0;
                Pc_alu <= '0;
                Pc_val <= '0;
                d_write_enable <= '0;
                d_load_enable <= '0;
              end
            else
              if(i_data_read[31:27] == 5b'00001)
                begin
                  Pc_cmd <= 2b'10;
                  Pc_alu <= 1;
                  Iv_alu <= 0;
                  d_write_enable <= 0;
                  d_load_enable  <= 0;
                  Rs1 <= 0;
                  Rs2 <= 0;
                  Pc_val <= 2b'01;
                  assign Iv = i_data_read[25:0];
                  if(i_data_read[26] == 1)
                    begin
                      I <= 4d'15;
                      Rd <= 5d'31;
                    end
                  else
                    begin
                      I <= 4d'0;
                      Rd <= 5d'0;
                    end
                end
              else
                begin
                  Rs1 <= i_data_read[25:21];
                  Rs2 <= i_data_read[20:16];
                  Rd  <= i_data_read[20:16];
                  d_load_enable = 0;
                  d_write_enable = 0;
                  Pc_cmd = 0;
                  Pc_val = 0;
                  Iv_alu = 1;
                  Iv = 0;
                  case(i_data_read[31:26])
                    0x08 :
                      begin
                        I = 4d'1;
                        assign Iv = i_data_read[15:0];
                      end
                    0x0c :
                      begin

                      end
                    0x04 :
                      begin

                      end
                    0x05 :
                      begin

                      end
                    0x13 :
                      begin

                      end
                    0x12 :
                      begin

                      end
                    0x0f :
                      begin

                      end
                    0x23 :
                      begin

                      end
                    0x0d :
                      begin

                      end
                    0x18 :
                      begin

                      end
                    0x1c :
                      begin

                      end
                    0x14 :
                      begin

                      end
                    0x1a :
                      begin

                      end
                    0x19 :
                      begin

                      end
                    0x17 :
                      begin

                      end
                    0x16 :
                      begin

                      end
                    0x0a :
                      begin

                      end
                    0x2b :
                      begin

                      end
                    0x0e :
                      begin

                      end
                end
