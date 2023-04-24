#!/bin/bash
#!/bin/bash
#SBATCH --job-name=all_runs 
#SBATCH --cpus-per-task=1 
#SBATCH --error="all_runs.err" 
#SBATCH --output=all_runs.log 
#SBATCH --nodelist=slurm1

set -e

# aws ec2 delete-fpga-image --fpga-image-id afi-04be7fca91e251d3f
# aws ec2 delete-fpga-image --fpga-image-id afi-08fc32f1697f9d885
# aws ec2 delete-fpga-image --fpga-image-id afi-0a9062d4edcfad10c
# aws ec2 delete-fpga-image --fpga-image-id afi-0d1d6218bb45091e2
# aws ec2 delete-fpga-image --fpga-image-id afi-03c6509d01bd1c059
# aws ec2 delete-fpga-image --fpga-image-id afi-0902452681a305601
# aws ec2 delete-fpga-image --fpga-image-id afi-03fdd6b2e7f30b506
# aws ec2 delete-fpga-image --fpga-image-id afi-0013f07bfff92e8d5
# aws ec2 delete-fpga-image --fpga-image-id afi-0d5f7230988292207

# aws ec2 delete-fpga-image --fpga-image-id afi-0174a05d90a62ce50
# aws ec2 delete-fpga-image --fpga-image-id afi-00aacf49314bdfda1
# aws ec2 delete-fpga-image --fpga-image-id afi-08bcacea8e22b2dd3
# aws ec2 delete-fpga-image --fpga-image-id afi-00bea03c0f88cc702
# aws ec2 delete-fpga-image --fpga-image-id afi-028d500b625c32247
# aws ec2 delete-fpga-image --fpga-image-id afi-047e5da74c5999b69
# aws ec2 delete-fpga-image --fpga-image-id afi-06267e546683cc130

# aws ec2 delete-fpga-image --fpga-image-id afi-093a8db8fd661916d
# aws ec2 delete-fpga-image --fpga-image-id afi-00b8eeb27dde2cf9f
# aws ec2 delete-fpga-image --fpga-image-id afi-0ddbb555f854be365
# aws ec2 delete-fpga-image --fpga-image-id afi-0ddbb555f854be365
# aws ec2 delete-fpga-image --fpga-image-id afi-094cf9c890e806854
# aws ec2 delete-fpga-image --fpga-image-id afi-0a11d08213d2e3235
# aws ec2 delete-fpga-image --fpga-image-id afi-03bdc4b5f607d48bf

# aws ec2 delete-fpga-image --fpga-image-id afi-0ddc4a15020fc21cc
# aws ec2 delete-fpga-image --fpga-image-id afi-0cb35f9cd10b6dbe2
# aws ec2 delete-fpga-image --fpga-image-id afi-0a811fce161b564b6
# aws ec2 delete-fpga-image --fpga-image-id afi-0658ca69ed4348262
# aws ec2 delete-fpga-image --fpga-image-id afi-069d8bb54c8f54e99
# aws ec2 delete-fpga-image --fpga-image-id afi-0c3bad39692e5862d
# aws ec2 delete-fpga-image --fpga-image-id afi-096ad64a0d0a3619c
# aws ec2 delete-fpga-image --fpga-image-id afi-07e031c56e31298bc

# aws ec2 delete-fpga-image --fpga-image-id afi-01f94c10af60fb508
# aws ec2 delete-fpga-image --fpga-image-id afi-00403471f706d1c75
# aws ec2 delete-fpga-image --fpga-image-id afi-09bff908a4165f264
# aws ec2 delete-fpga-image --fpga-image-id afi-0acc373fb85a448f8
# aws ec2 delete-fpga-image --fpga-image-id afi-014685ec7d6dbad77
# aws ec2 delete-fpga-image --fpga-image-id afi-0b1bd9b3e34f4e87b
# aws ec2 delete-fpga-image --fpga-image-id afi-098b4535cac6e55a4
# aws ec2 delete-fpga-image --fpga-image-id afi-055b645920513a4b7

# aws ec2 delete-fpga-image --fpga-image-id afi-09499989b96f27e3e
# aws ec2 delete-fpga-image --fpga-image-id afi-01138dfe762f2733d
# aws ec2 delete-fpga-image --fpga-image-id afi-06df68b5610564407
# aws ec2 delete-fpga-image --fpga-image-id afi-020decc56a7e8ea30
# aws ec2 delete-fpga-image --fpga-image-id afi-06ae41163482de70a
# aws ec2 delete-fpga-image --fpga-image-id afi-057b4d4f3caa31070
# aws ec2 delete-fpga-image --fpga-image-id afi-0dfcabd8eb5ee6e08

# aws ec2 delete-fpga-image --fpga-image-id afi-0cd39e2d22dbf30de
# aws ec2 delete-fpga-image --fpga-image-id afi-0ec4476c79003dd48
# aws ec2 delete-fpga-image --fpga-image-id afi-036168c6094f637c0
# aws ec2 delete-fpga-image --fpga-image-id afi-0a89de7d3c17ea133
# aws ec2 delete-fpga-image --fpga-image-id afi-0c2bab5a7ac4561a0
# aws ec2 delete-fpga-image --fpga-image-id afi-05d5384e24fbfb17c

# aws ec2 delete-fpga-image --fpga-image-id afi-06f28438f38742eb5
# aws ec2 delete-fpga-image --fpga-image-id afi-057753463bd92cd94
# aws ec2 delete-fpga-image --fpga-image-id afi-0cc8b3ef11d46115f
# aws ec2 delete-fpga-image --fpga-image-id afi-0093df7a759b288a6
# aws ec2 delete-fpga-image --fpga-image-id afi-0e9de3386dbcfff2f
# aws ec2 delete-fpga-image --fpga-image-id afi-0927fdb08cfa7c304
# aws ec2 delete-fpga-image --fpga-image-id afi-02ae91166d239c62c
# aws ec2 delete-fpga-image --fpga-image-id afi-0c87f65e98edbab12

# aws ec2 delete-fpga-image --fpga-image-id afi-0f85562629db060d5
# aws ec2 delete-fpga-image --fpga-image-id afi-0faa0a0c444f061cc
# aws ec2 delete-fpga-image --fpga-image-id afi-0efaf0187db27583e
# aws ec2 delete-fpga-image --fpga-image-id afi-01cebcc20fadf2fff
# aws ec2 delete-fpga-image --fpga-image-id afi-052243e2c23149cf4

# aws ec2 delete-fpga-image --fpga-image-id afi-0d77502fddf16b3a2
# aws ec2 delete-fpga-image --fpga-image-id afi-042fc883b53da2229
# aws ec2 delete-fpga-image --fpga-image-id afi-012cbf5ff7d792341
# aws ec2 delete-fpga-image --fpga-image-id afi-03de04896fbccf809
# aws ec2 delete-fpga-image --fpga-image-id afi-0b0aa16f5b8fb2901
# aws ec2 delete-fpga-image --fpga-image-id afi-00b6cf41cc4e794c6
# aws ec2 delete-fpga-image --fpga-image-id afi-0495c39b3802aa48e
# aws ec2 delete-fpga-image --fpga-image-id afi-07bcc8d8ef62fa480
# aws ec2 delete-fpga-image --fpga-image-id afi-006a11e9a4b05486e
# aws ec2 delete-fpga-image --fpga-image-id afi-098990c7f9e73ee3a
# aws ec2 delete-fpga-image --fpga-image-id afi-0ba621ad6962cc334
# aws ec2 delete-fpga-image --fpga-image-id afi-0b2ad5c822870bcbd
# aws ec2 delete-fpga-image --fpga-image-id afi-095ec81e6afe3a191
# aws ec2 delete-fpga-image --fpga-image-id afi-0781622b101030409
# aws ec2 delete-fpga-image --fpga-image-id afi-099b11eb177a63705
# aws ec2 delete-fpga-image --fpga-image-id afi-0d498a82597fed2a2
# aws ec2 delete-fpga-image --fpga-image-id afi-07e43a6eb1c29e38d
# aws ec2 delete-fpga-image --fpga-image-id afi-08b70ac24f3280c83
# aws ec2 delete-fpga-image --fpga-image-id afi-024d995130862ce25
# aws ec2 delete-fpga-image --fpga-image-id afi-0490f8b38aae08624
# aws ec2 delete-fpga-image --fpga-image-id afi-0919d8ac50a089e16
# aws ec2 delete-fpga-image --fpga-image-id afi-02f46116e915c46b8
# aws ec2 delete-fpga-image --fpga-image-id afi-0f74f03da984c8b25
# aws ec2 delete-fpga-image --fpga-image-id afi-0a31e1dac6e55189d
# aws ec2 delete-fpga-image --fpga-image-id afi-0548f1ec38547bff6
# aws ec2 delete-fpga-image --fpga-image-id afi-0db62e4563e094cf0
# aws ec2 delete-fpga-image --fpga-image-id afi-091ddfe05562a3fc5
# aws ec2 delete-fpga-image --fpga-image-id afi-0264a05d17b5bbdcd
# aws ec2 delete-fpga-image --fpga-image-id afi-0264a05d17b5bbdcd
# aws ec2 delete-fpga-image --fpga-image-id afi-0264a05d17b5bbdcd

echo 'finished: successfully finished all processing'





