from osrf_pycommon.process_utils.async_execute_process import async_execute_process
from osrf_pycommon.process_utils import get_loop

from .impl_aep_protocol import create_protocol

loop = get_loop()


async def run(cmd, **kwargs):
    transport, protocol = await async_execute_process(
        create_protocol(), cmd, **kwargs)
    retcode = await protocol.complete
    transport.close()
    return protocol.stdout_buffer, protocol.stderr_buffer, retcode
