from osrf_pycommon.process_utils import AsyncSubprocessProtocol


def create_protocol():
    class CustomProtocol(AsyncSubprocessProtocol):
        def __init__(self, **kwargs):
            self.stdout_buffer = b""
            self.stderr_buffer = b""
            AsyncSubprocessProtocol.__init__(self, **kwargs)

        def on_stdout_received(self, data):
            self.stdout_buffer += data

        def on_stderr_received(self, data):
            self.stderr_buffer += data
    return CustomProtocol
